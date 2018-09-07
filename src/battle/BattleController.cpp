#include "battle/BattleController.hpp"

#include <cassert>
#include "battle/data/BoundMove.hpp"
#include "battle/data/Flag.hpp"
#include "battle/data/Move.hpp"
#include "battle/data/Pokemon.hpp"
#include "battle/data/PokemonSpeciesData.hpp"
#include "battle/helpers/battle-utils.hpp"
#include "battle/helpers/move-effects.hpp"
#include "battle/helpers/random.hpp"
#include "battle/TextProvider.hpp"
#include "components/battle/Battle.hpp"
#include "components/battle/Fainted.hpp"
#include "components/battle/VolatileData.hpp"
#include "constants.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "events/ImmediateEvent.hpp"
#include "events/TextEvent.hpp"
#include "EventQueue.hpp"
#include "lua-native-functions.hpp"

using engine::entitysystem::Entity;

namespace {
    template<typename TEvent, typename... Args>
    void enqueueTurnEvent(CoreStructures& gameData, Args&&... args) {
        EventQueue& queue = resource<EventQueue>("battle-event-queue", gameData);
        queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
    }

    template<typename TEvent, typename... Args>
    void enqueueMoveEvent(CoreStructures& gameData, Args&&... args) {
        EventQueue& queue = resource<EventQueue>("move-event-queue", gameData);
        queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
    }

    template<typename TKey>
    void updateFlagTable(std::unordered_map<TKey, std::vector<Flag>>& flagTable) {
        for (auto& [_, flagVector] : flagTable) {
            auto it = flagVector.begin();
            while (it != flagVector.end()) {
                it->duration--;
                if (it->duration == 0) {
                    it = flagVector.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
}

BattleController::BattleController(
    Entity battleEntity,
    TextProvider& textProvider,
    CoreStructures& gameData
) : battleEntity(battleEntity),
    textProvider(&textProvider),
    gameData(&gameData),
    scriptVariables(gameData),
    eventManager(scriptVariables, gameData),
    state(State::PENDING_START) { }

void BattleController::startBattle() {
    assert(state == State::PENDING_START);
    gameData->resourceStorage->store("battle-event-queue", EventQueue());
    gameData->resourceStorage->store("move-event-queue", EventQueue());
    battle = &data<Battle>(battleEntity, *gameData);
    scriptVariables.setBattle(*battle);
    eventManager.setBattle(*battle);
    loadDetailedPokemonData();

    lua::internal::setBattle(battleEntity);
    effects::internal::setGameData(*gameData);
    effects::internal::setBattle(battleEntity);
    effects::internal::setTriggerEvent([&](const std::string& eventName) {
        eventManager.triggerEvent(eventName);
    });

    state = State::READY;
}

void BattleController::abort() {
    gameData->resourceStorage->remove<EventQueue>("battle-event-queue");
    gameData->resourceStorage->remove<EventQueue>("move-event-queue");
    state = State::PENDING_START;
}

void BattleController::tick() {
    assert(state == State::READY);
    auto& moveQueue = resource<EventQueue>("move-event-queue", *gameData);

    if (!moveQueue.empty()) {
        moveQueue.tick();
        return;
    }

    resource<EventQueue>("battle-event-queue", *gameData).tick();
}

bool BattleController::hasPendingEvents() const {
    return !resource<EventQueue>("move-event-queue", *gameData).empty()
        || !resource<EventQueue>("battle-event-queue", *gameData).empty();
}

BattleController::State BattleController::getState() const {
    return state;
}

int BattleController::chooseMoveAI(const Pokemon& pokemon) {
    assert(state == State::READY);
    scriptVariables.updateScriptVariables();
    return script("ai", *gameData).call<int>("chooseMoveWildBattle");
}

void BattleController::processTurn(const std::vector<BoundMove>& usedMoves) {
    assert(state == State::READY);

    battle->usedMoves = usedMoves;
    sortUsedMoves(battle->usedMoves);
    eventManager.triggerEvent("onTurnStart");
    processUsedMoves();

    enqueueTurnEvent<ImmediateEvent>(*gameData, [this] {
        eventManager.triggerEvent("onTurnEnd");
        updateActiveFlags();
    });
}

void BattleController::sortUsedMoves(std::vector<BoundMove>& usedMoves) {
    // Ensures random move order if both the priority and speed are equal
    shuffle(usedMoves.begin(), usedMoves.end());

    std::sort(
        usedMoves.begin(),
        usedMoves.end(),
        [&](const BoundMove& first, const BoundMove& second) {
            if (first.move->priority != second.move->priority) {
                return first.move->priority > second.move->priority;
            }

            int firstSpeed = getEffectiveStat(first.user, Stat::Speed, *gameData);
            int secondSpeed = getEffectiveStat(second.user, Stat::Speed, *gameData);
            return firstSpeed >= secondSpeed;
        }
    );
}

void BattleController::updateActiveFlags() {
    updateFlagTable(battle->playerTeamPositionFlags);
    updateFlagTable(battle->opponentTeamPositionFlags);
    updateFlagTable(battle->pokemonFlags);
}

void BattleController::processUsedMoves() {
    for (const auto& boundMove : battle->usedMoves) {
        processMove(boundMove);
    }
}

void BattleController::processMove(BoundMove boundMove) {
    enqueueTurnEvent<ImmediateEvent>(*gameData, [&, boundMove] {
        if (pokemon(boundMove.user).currentHP <= 0) {
            return;
        }

        prepareScriptsForMove(boundMove);

        eventManager.triggerUserEvents(boundMove, "beforeMove");

        enqueueMoveEvent<ImmediateEvent>(*gameData, [&] {
            if (effects::isMoveNegated()) {
                effects::cleanup();
                return;
            }

            showUsedMoveText(boundMove);
            deductPPIfApplicable(boundMove);
            // TODO: move animation?
            processMoveEffects(boundMove);
        });
    });
}

void BattleController::prepareScriptsForMove(const BoundMove& boundMove) {
    effects::internal::setMove(boundMove);
    scriptVariables.updateScriptUserPointer(boundMove.user);
    scriptVariables.updateScriptTargetPointer(boundMove.target);
}

void BattleController::showUsedMoveText(const BoundMove& boundMove) {
    std::string userDisplayName = pokemon(boundMove.user).displayName;

    if (isOpponent(boundMove.user)) {
        userDisplayName = "Foe " + userDisplayName;
    }

    showText(userDisplayName + " used " + boundMove.move->displayName + "!");
}

void BattleController::deductPPIfApplicable(const BoundMove& usedMove) {
    if (usedMove.moveIndex >= 0 && usedMove.moveIndex < constants::MOVE_LIMIT) {
        pokemon(usedMove.user).pp[usedMove.moveIndex]--;
    }
}

void BattleController::processMoveEffects(const BoundMove& usedMove) {
    Entity user = usedMove.user;
    Entity target = usedMove.target;
    Move& move = *usedMove.move;

    if (checkMiss(user, target, move, *gameData)) {
        std::string& displayName = pokemon(user).displayName;
        showText(displayName + "'s attack missed!");
        return;
    }

    if (checkCritical(user, target, move, *gameData)) {
        effects::ensureCriticalHit();
    }

    switch (move.functionCode) {
        case 0:
            effects::damage();
            break;
        case -1:
        case -2:
        case -3:
            effects::lowerStat(move.functionParameter, -move.functionCode);
            break;
        case 1:
        case 2:
        case 3:
            effects::raiseStat(move.functionParameter, move.functionCode);
            break;
        case 6:
            effects::damageWithRecoil(move.functionParameter / 100.0);
            break;
        case 8:
            effects::fixedDamage(move.functionParameter);
            break;
        case 9:
            effects::fixedDamage(data<Pokemon>(target, *gameData).currentHP);
            break;
        case 99:
            eventManager.triggerUserEvents(usedMove, "onUse");
            break;
    }

    enqueueMoveEvent<ImmediateEvent>(*gameData, effects::cleanup);
    checkFaintedPokemon();
}

void BattleController::checkFaintedPokemon() {
    enqueueMoveEvent<ImmediateEvent>(*gameData, [&] {
        std::vector<Entity> pokemonList = getPokemonList(*battle);

        for (const auto& pokemonEntity : pokemonList) {
            const Pokemon& currentPokemon = pokemon(pokemonEntity);

            if (currentPokemon.currentHP <= 0) {
                showText(currentPokemon.displayName + " fainted!");
                enqueueMoveEvent<ImmediateEvent>(*gameData, [&, pokemonEntity] {
                    // TODO: fainting animation?
                    addComponent(pokemonEntity, Fainted{}, *gameData);
                });
            }
        }

        enqueueMoveEvent<ImmediateEvent>(*gameData, [this] {
            bool playerLost = isPlayerUnableToContinue();
            bool opponentLost = isOpponentUnableToContinue();

            if (playerLost || opponentLost) {
                resource<EventQueue>("move-event-queue", *gameData) = {};
                resource<EventQueue>("battle-event-queue", *gameData) = {};

                state = playerLost
                    ? (opponentLost ? State::DRAW : State::DEFEAT)
                    : State::VICTORY;
            }
        });
    });
}

void BattleController::showText(const std::string& content) {
    textProvider->showMoveText(content, battleEntity, *gameData);
}

bool BattleController::isPlayerUnableToContinue() const {
    // TODO: handle benched Pokémon
    return isEveryPokemonFainted(battle->playerTeam);
}

bool BattleController::isOpponentUnableToContinue() const {
    // TODO: handle benched Pokémon
    return isEveryPokemonFainted(battle->opponentTeam);
}

bool BattleController::isEveryPokemonFainted(const std::vector<Entity>& team) const {
    for (const auto& pokemonEntity : team) {
        if (!hasComponent<Fainted>(pokemonEntity, *gameData)) {
            return false;
        }
    }

    return true;
}

bool BattleController::isOpponent(Entity entity) const {
    for (const auto& opponent : battle->opponentTeam) {
        if (entity == opponent) {
            return true;
        }
    }

    return false;
}

Pokemon& BattleController::pokemon(Entity entity) {
    return data<Pokemon>(entity, *gameData);
}

void BattleController::loadDetailedPokemonData() {
    std::vector<Entity> pokemonList = getPokemonList(*battle);

    for (const auto& pokemonEntity : pokemonList) {
        loadMoves(pokemonEntity);
        loadSpeciesData(pokemonEntity);
        addComponent(pokemonEntity, VolatileData{}, *gameData);
    }
}

void BattleController::loadMoves(Entity pokemonEntity) {
    const Pokemon& currentPokemon = pokemon(pokemonEntity);
    std::vector<Move*> moveList;

    for (size_t i = 0; i < currentPokemon.moves.size(); ++i) {
        Move& move = resource<Move>("move-" + currentPokemon.moves[i], *gameData);
        moveList.push_back(&move);
    }

    addComponent(pokemonEntity, moveList, *gameData);
}

void BattleController::loadSpeciesData(Entity pokemonEntity) {
    const Pokemon& currentPokemon = pokemon(pokemonEntity);
    PokemonSpeciesData& speciesData = resource<PokemonSpeciesData>(
        "pokemon-" + currentPokemon.species,
        *gameData
    );

    addComponent(pokemonEntity, speciesData, *gameData);
}
