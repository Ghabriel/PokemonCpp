#include "battle/BattleController.hpp"

#include <cassert>
#include "battle/data/Pokemon.hpp"
#include "battle/data/PokemonSpeciesData.hpp"
#include "battle/helpers/battle-utils.hpp"
#include "battle/helpers/move-effects.hpp"
#include "battle/helpers/random.hpp"
#include "battle/TextProvider.hpp"
#include "components/battle/Battle.hpp"
#include "components/battle/Fainted.hpp"
#include "components/battle/VolatileData.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "events/ImmediateEvent.hpp"
#include "events/TextEvent.hpp"
#include "EventQueue.hpp"
#include "lua-native-functions.hpp"

using engine::entitysystem::Entity;

namespace {
    template<typename TEvent, typename... Args>
    void enqueueEvent(CoreStructures& gameData, Args&&... args) {
        EventQueue& queue = resource<EventQueue>("battle-event-queue", gameData);
        queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
    }

    template<typename TEvent, typename... Args>
    void enqueueMoveEvent(CoreStructures& gameData, Args&&... args) {
        EventQueue& queue = resource<EventQueue>("move-event-queue", gameData);
        queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
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

void BattleController::processTurn(const std::deque<UsedMove>& usedMoves) {
    assert(state == State::READY);

    battle->usedMoves = usedMoves;
    sortUsedMoves(battle->usedMoves);
    eventManager.triggerEvent("onTurnStart");
    processUsedMoves();

    enqueueEvent<ImmediateEvent>(*gameData, [this] {
        eventManager.triggerEvent("onTurnEnd");
        updateActiveMoveList();
    });
}

void BattleController::sortUsedMoves(std::deque<UsedMove>& usedMoves) {
    shuffle(usedMoves.begin(), usedMoves.end());

    std::sort(
        usedMoves.begin(),
        usedMoves.end(),
        [&](const UsedMove& first, const UsedMove& second) {
            if (first.move->priority != second.move->priority) {
                return first.move->priority > second.move->priority;
            }

            int firstSpeed = getEffectiveStat(first.user, Stat::Speed, *gameData);
            int secondSpeed = getEffectiveStat(second.user, Stat::Speed, *gameData);
            return firstSpeed >= secondSpeed;
        }
    );
}

void BattleController::updateActiveMoveList() {
    std::vector<std::pair<UsedMove, int>> newList;

    for (auto& [usedMove, duration] : battle->activeMoves) {
        if (duration != 0) {
            newList.push_back({std::move(usedMove), duration - 1});
        }
    }

    battle->activeMoves.swap(newList);
}

void BattleController::processUsedMoves() {
    for (const auto& usedMove : battle->usedMoves) {
        processMove(usedMove);
    }
}

void BattleController::processMove(UsedMove usedMove) {
    enqueueEvent<ImmediateEvent>(*gameData, [&, usedMove] {
        if (pokemon(usedMove.user).currentHP <= 0) {
            return;
        }

        effects::internal::setMoveUser(usedMove.user);
        effects::internal::setMoveTarget(usedMove.target);
        effects::internal::setMove(*usedMove.move);
        effects::internal::setUsedMove(usedMove);
        scriptVariables.updateScriptUserPointer(usedMove.user);
        scriptVariables.updateScriptTargetPointer(usedMove.target);

        eventManager.triggerMoveEvent(usedMove, "beforeMove");

        enqueueMoveEvent<ImmediateEvent>(*gameData, [&] {
            if (effects::isMoveNegated()) {
                effects::cleanup();
                return;
            }

            showUsedMoveText(usedMove);
            deductPPIfApplicable(usedMove);
            // TODO: move animation?
            processMoveEffects(usedMove);
        });
    });
}

void BattleController::showUsedMoveText(const UsedMove& usedMove) {
    std::string userDisplayName = pokemon(usedMove.user).displayName;

    if (usedMove.user == battle->opponentPokemon) {
        userDisplayName = "Foe " + userDisplayName;
    }

    showMoveText(userDisplayName + " used " + usedMove.move->displayName + "!");
}

void BattleController::deductPPIfApplicable(const UsedMove& usedMove) {
    if (usedMove.move->id != "Struggle") {
        pokemon(usedMove.user).pp[usedMove.moveIndex]--;
    }
}

void BattleController::processMoveEffects(const UsedMove& usedMove) {
    Entity user = usedMove.user;
    Entity target = usedMove.target;
    Move& move = *usedMove.move;

    if (checkMiss(user, target, move, *gameData)) {
        std::string& displayName = pokemon(user).displayName;
        showMoveText(displayName + "'s attack missed!");
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
            eventManager.triggerMoveEvent(usedMove, "onUse");
            break;
    }

    enqueueMoveEvent<ImmediateEvent>(*gameData, effects::cleanup);
    checkFaintedPokemon();
}

void BattleController::checkFaintedPokemon() {
    enqueueMoveEvent<ImmediateEvent>(*gameData, [&] {
        std::vector<Entity> pokemonList = {
            battle->playerPokemon,
            battle->opponentPokemon
        };

        for (const auto& pokemonEntity : pokemonList) {
            const Pokemon& currentPokemon = pokemon(pokemonEntity);

            if (currentPokemon.currentHP <= 0) {
                showMoveText(currentPokemon.displayName + " fainted!");
                // TODO fainting animation?
                addComponent(pokemonEntity, Fainted{}, *gameData);

                enqueueMoveEvent<ImmediateEvent>(*gameData, [this, pokemonEntity] {
                    resource<EventQueue>("move-event-queue", *gameData) = {};
                    resource<EventQueue>("battle-event-queue", *gameData) = {};

                    if (pokemonEntity == battle->playerPokemon) {
                        state = State::DEFEAT;
                    } else {
                        state = State::VICTORY;
                    }
                });

                // TODO: handle multiple knock-outs (may happen via recoil moves/Explosion)
                break;
            }
        }
    });
}

void BattleController::showText(const std::string& content) {
    textProvider->showBattleText(content, battleEntity, *gameData);
}

void BattleController::showMoveText(const std::string& content) {
    textProvider->showMoveText(content, battleEntity, *gameData);
}

Pokemon& BattleController::pokemon(Entity entity) {
    return data<Pokemon>(entity, *gameData);
}

void BattleController::loadDetailedPokemonData() {
    std::vector<Entity> pokemonList = {
        battle->playerPokemon,
        battle->opponentPokemon
    };

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
