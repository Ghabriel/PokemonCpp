#include "states/BattleState.hpp"

#include <queue>
#include "battle/battle-setup.hpp"
#include "battle/battle-utils.hpp"
#include "battle/events/ActionSelectionEvent.hpp"
#include "battle/Move.hpp"
#include "battle/move-effects.hpp"
#include "battle/Pokemon.hpp"
#include "battle/PokemonSpeciesData.hpp"
#include "battle/random.hpp"
#include "components/battle/Battle.hpp"
#include "components/battle/BattleActionSelection.hpp"
#include "components/battle/BattleMoveSelection.hpp"
#include "components/battle/Fainted.hpp"
#include "components/battle/VolatileData.hpp"
#include "components/TextBox.hpp"
#include "constants.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "events/ImmediateEvent.hpp"
#include "events/TextEvent.hpp"
#include "EventQueue.hpp"
#include "lua-native-functions.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::entitysystem::Entity;
using engine::inputsystem::InputContext;
using engine::scriptingsystem::Lua;

namespace {
    enum class BattleAction {
        Fight,
        Bag,
        Pokemon,
        Run
    };

    constexpr int STRUGGLE = -2;

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

    void injectNativeBattleVariables(
        const std::unordered_map<std::string, Entity>& pokemonList,
        Lua& script,
        CoreStructures& gameData
    ) {
        for (const auto& [varName, pokemonEntity] : pokemonList) {
            const Pokemon& currentPokemon = data<Pokemon>(pokemonEntity, gameData);

            const auto getStat = [&](Stat stat) {
                return getEffectiveStat(pokemonEntity, stat, gameData);
            };

            script.set(varName + ".species", currentPokemon.species);
            script.set(varName + ".nature", static_cast<int>(currentPokemon.nature));
            script.set(varName + ".heldItem", currentPokemon.heldItem);
            script.set(varName + ".ability", currentPokemon.ability);

            for (size_t i = 0; i < constants::MOVE_LIMIT; ++i) {
                if (i < currentPokemon.moves.size()) {
                    script.set(varName + ".move" + std::to_string(i), currentPokemon.moves[i]);
                    script.set(varName + ".pp" + std::to_string(i), currentPokemon.pp[i]);
                } else {
                    script.set(varName + ".move" + std::to_string(i), std::string{});
                    script.set(varName + ".pp" + std::to_string(i), std::string{});
                }
            }

            script.set<int>(varName + ".moveCount", currentPokemon.moves.size());

            script.set(varName + ".gender", static_cast<int>(currentPokemon.gender));
            script.set(varName + ".form", currentPokemon.form);
            script.set(varName + ".displayName", currentPokemon.displayName);
            script.set(varName + ".status", static_cast<int>(currentPokemon.status));
            script.set(varName + ".asleepRounds", currentPokemon.asleepRounds);
            script.set(varName + ".level", currentPokemon.level);
            script.set(varName + ".hp", getStat(Stat::HP));
            script.set(varName + ".attack", getStat(Stat::Attack));
            script.set(varName + ".defense", getStat(Stat::Defense));
            script.set(varName + ".specialAttack", getStat(Stat::SpecialAttack));
            script.set(varName + ".specialDefense", getStat(Stat::SpecialDefense));
            script.set(varName + ".speed", getStat(Stat::Speed));
            script.set(varName + ".currentHP", static_cast<int>(currentPokemon.currentHP));
        }
    }
}

BattleState::BattleState(CoreStructures& gameData)
 : gameData(gameData),
   battleEntity(createEntity(gameData)) {
    lua::internal::setBattle(battleEntity);
    effects::internal::setGameData(gameData);
    effects::internal::setBattle(battleEntity);
}

void BattleState::onEnterImpl() {
    restoreEntity(battleEntity, gameData);
    setupWildEncounter("map-basic", battleEntity, gameData);
    gameData.resourceStorage->store("battle-event-queue", EventQueue());
    gameData.resourceStorage->store("move-event-queue", EventQueue());
    battle = &data<Battle>(battleEntity, gameData);
    loadDetailedPokemonData();

    showText("Wild " + pokemon(battle->opponentPokemon).displayName + " appeared!");
    // TODO: only show the player's pokémon now
    actionSelectionScreen();
}

void BattleState::onExitImpl() {
    music("bgm-wild-battle", gameData).stop();
    deleteEntity(battleEntity, gameData);
    gameData.componentManager->cleanup<Battle, Pokemon>();
    gameData.resourceStorage->remove<EventQueue>("battle-event-queue");
    gameData.resourceStorage->remove<EventQueue>("move-event-queue");
}

void BattleState::executeImpl() {
    auto& moveQueue = resource<EventQueue>("move-event-queue", gameData);

    if (!moveQueue.empty()) {
        moveQueue.tick();
        return;
    }

    resource<EventQueue>("battle-event-queue", gameData).tick();
}

void BattleState::actionSelectionScreen() {
    enqueueEvent<ImmediateEvent>(gameData, [&] {
        addComponent(
            battleEntity,
            BattleActionSelection{
                "What will " + pokemon(battle->playerPokemon).displayName + " do?",
                {"Fight", "Bag", "Pokemon", "Run"},
                300,
                0
            },
            gameData
        );
    });

    enqueueEvent<ActionSelectionEvent>(
        gameData,
        selectedAction,
        false,
        [&]() -> size_t& {
            return data<BattleActionSelection>(battleEntity, gameData).focusedOption;
        },
        [](size_t) { return true; },
        gameData
    );

    enqueueEvent<ImmediateEvent>(gameData, [&] {
        removeComponent<BattleActionSelection>(battleEntity, gameData);

        switch (static_cast<BattleAction>(selectedAction)) {
            case BattleAction::Fight:
                if (!hasUsableMoves(battle->playerPokemon, gameData)) {
                    std::string& displayName = pokemon(battle->playerPokemon).displayName;
                    showText(displayName + " has no moves left!");
                    selectedAction = STRUGGLE;
                    processTurn();
                    return;
                }

                moveSelectionScreen();
                break;
            case BattleAction::Bag:
                showText("TODO: bag");
                actionSelectionScreen();
                break;
            case BattleAction::Pokemon:
                showText("TODO: pokemon");
                actionSelectionScreen();
                break;
            case BattleAction::Run:
                // TODO: check if the escape succeeds
                sound("fx-battle-run", gameData).play();
                showText("Got away safely!");
                enqueueEvent<ImmediateEvent>(gameData, [&] {
                    gameData.stateMachine->pushState("overworld-state");
                });
                break;
        }
    });
}

void BattleState::moveSelectionScreen() {
    enqueueEvent<ImmediateEvent>(gameData, [&] {
        BattleMoveSelection moveSelection;
        const Pokemon& currentPokemon = pokemon(battle->playerPokemon);
        const auto& moveList = moves(battle->playerPokemon);
        for (size_t i = 0; i < currentPokemon.moves.size(); ++i) {
            const auto& move = *moveList[i];
            int maxPP = move.pp * (1 + 0.2 * currentPokemon.ppUps[i]);

            moveSelection.moves[i] = {{
                move.displayName,
                move.type, // TODO: display name of type?
                currentPokemon.pp[i],
                maxPP
            }};
        }

        moveSelection.optionBoxWidth = 300;
        moveSelection.focusedOption = 0;
        addComponent(battleEntity, moveSelection, gameData);
    });

    enqueueEvent<ActionSelectionEvent>(
        gameData,
        selectedAction,
        true,
        [&]() -> size_t& {
            return data<BattleMoveSelection>(battleEntity, gameData).focusedOption;
        },
        [&](size_t option) {
            return option < pokemon(battle->playerPokemon).moves.size();
        },
        gameData
    );

    enqueueEvent<ImmediateEvent>(gameData, [&] {
        removeComponent<BattleMoveSelection>(battleEntity, gameData);

        if (selectedAction == -1) {
            actionSelectionScreen();
            return;
        }

        if (pokemon(battle->playerPokemon).pp[selectedAction] == 0) {
            showText("That move has no PP left!"); // TODO
            moveSelectionScreen();
            return;
        }

        processTurn();
    });
}

void BattleState::processTurn() {
    enqueueEvent<ImmediateEvent>(gameData, [&] {
        const Move& playerMove = (selectedAction == STRUGGLE)
            ? resource<Move>("move-Struggle", gameData)
            : *moves(battle->playerPokemon)[selectedAction];
        int playerSpeed = getEffectiveStat(battle->playerPokemon, Stat::Speed, gameData);

        Pokemon& opponentPokemon = pokemon(battle->opponentPokemon);
        size_t opponentMoveIndex = chooseMoveAI(opponentPokemon);
        const Move& opponentMove = *moves(battle->opponentPokemon)[opponentMoveIndex];
        int opponentSpeed = getEffectiveStat(battle->opponentPokemon, Stat::Speed, gameData);

        if (playerMove.priority > opponentMove.priority) {
            processPlayerMove(selectedAction);
            processOpponentMove(opponentMoveIndex);
        } else if (playerMove.priority < opponentMove.priority) {
            processOpponentMove(opponentMoveIndex);
            processPlayerMove(selectedAction);
        } else if (playerSpeed > opponentSpeed) {
            processPlayerMove(selectedAction);
            processOpponentMove(opponentMoveIndex);
        } else if (playerSpeed < opponentSpeed) {
            processOpponentMove(opponentMoveIndex);
            processPlayerMove(selectedAction);
        } else if (random(1, 2) == 1) {
            processPlayerMove(selectedAction);
            processOpponentMove(opponentMoveIndex);
        } else {
            processOpponentMove(opponentMoveIndex);
            processPlayerMove(selectedAction);
        }

        actionSelectionScreen();
    });
}

void BattleState::updateAIVariables() {
    std::unordered_map<std::string, Entity> pokemonList = {
        {"user", battle->opponentPokemon},
        {"foe", battle->playerPokemon},
    };

    auto& ai = script("ai", gameData);
    injectNativeBattleVariables(pokemonList, ai, gameData);
}

size_t BattleState::chooseMoveAI(const Pokemon& pokemon) {
    updateAIVariables();
    return script("ai", gameData).call<int>("chooseMoveWildBattle");
}

void BattleState::processPlayerMove(int moveIndex) {
    enqueueEvent<ImmediateEvent>(gameData, [&, moveIndex] {
        Pokemon& user = pokemon(battle->playerPokemon);

        if (user.currentHP <= 0) {
            return;
        }

        Move& move = (moveIndex == STRUGGLE)
            ? resource<Move>("move-Struggle", gameData)
            : *moves(battle->playerPokemon)[moveIndex];
        showMoveText(user.displayName + " used " + move.displayName + "!");

        if (moveIndex != STRUGGLE) {
            user.pp[moveIndex]--;
        }

        // TODO: move animation?
        // TODO: handle non-single-target moves
        processMove(
            battle->playerPokemon,
            battle->opponentPokemon,
            move
        );
    });
}

void BattleState::processOpponentMove(int moveIndex) {
    enqueueEvent<ImmediateEvent>(gameData, [&, moveIndex] {
        Pokemon& user = pokemon(battle->opponentPokemon);

        if (user.currentHP <= 0) {
            return;
        }

        Move& move = (moveIndex == STRUGGLE)
            ? resource<Move>("move-Struggle", gameData)
            : *moves(battle->opponentPokemon)[moveIndex];
        showMoveText("Foe " + user.displayName + " used " + move.displayName + "!");

        if (moveIndex != STRUGGLE) {
            user.pp[moveIndex]--;
        }

        // TODO: move animation?
        // TODO: handle non-single-target moves
        processMove(
            battle->opponentPokemon,
            battle->playerPokemon,
            move
        );
    });
}

void BattleState::processMove(Entity user, Entity target, Move& move) {
    effects::internal::setMoveUser(user);
    effects::internal::setMoveTarget(target);
    effects::internal::setMove(move);

    if (checkMiss(user, target, move, gameData)) {
        std::string& displayName = pokemon(user).displayName;
        showMoveText(displayName + "'s attack missed!");
        return;
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
        case 99:
            updateMoveVariables(user, target);
            script("moves", gameData).call<void>(move.id + "_onUse");
            break;
    }

    checkFaintedPokemon();
}

void BattleState::updateMoveVariables(Entity user, Entity target) {
    std::unordered_map<std::string, Entity> pokemonList = {
        {"user", user},
        {"target", target},
    };

    auto& moves = script("moves", gameData);
    injectNativeBattleVariables(pokemonList, moves, gameData);
}

void BattleState::checkFaintedPokemon() {
    enqueueMoveEvent<ImmediateEvent>(gameData, [&] {
        std::vector<Entity> pokemonList = {
            battle->playerPokemon,
            battle->opponentPokemon
        };

        for (const auto& pokemonEntity : pokemonList) {
            const Pokemon& currentPokemon = pokemon(pokemonEntity);

            if (currentPokemon.currentHP <= 0) {
                showMoveText(currentPokemon.displayName + " fainted!");
                // TODO fainting animation?
                addComponent(pokemonEntity, Fainted{}, gameData);

                if (pokemonEntity == battle->playerPokemon) {
                    blackOutScreen();
                } else {
                    rewardScreen();
                }
            }
        }
    });
}

void BattleState::blackOutScreen() {
    showMoveText("You blacked out!");
    // TODO: move the player somewhere safe (in the Overworld)
    enqueueMoveEvent<ImmediateEvent>(gameData, [&] {
        gameData.stateMachine->pushState("overworld-state");
    });
}

void BattleState::rewardScreen() {
    resource<EventQueue>("battle-event-queue", gameData).clear();

    enqueueEvent<ImmediateEvent>(gameData, [&] {
        music("bgm-wild-battle", gameData).stop();
        music("bgm-wild-battle-victory", gameData).play();
    });

    Pokemon& playerPokemon = pokemon(battle->playerPokemon);
    int exp = calculateExpGain(
        playerPokemon,
        pokemon(battle->opponentPokemon),
        species(battle->opponentPokemon)
    );
    showText(playerPokemon.displayName + " gained " + std::to_string(exp) + " EXP. Points!");

    enqueueEvent<ImmediateEvent>(gameData, [&] {
        deleteEntity(battle->opponentPokemon, gameData);
        music("bgm-wild-battle-victory", gameData).stop();
        gameData.stateMachine->pushState("overworld-state");
    });
}

void BattleState::showText(const std::string& content) {
    enqueueEvent<TextEvent>(gameData, content, battleEntity, gameData);
}

void BattleState::showMoveText(const std::string& content) {
    enqueueMoveEvent<TextEvent>(gameData, content, battleEntity, gameData);
}

Pokemon& BattleState::pokemon(Entity entity) {
    return data<Pokemon>(entity, gameData);
}

std::vector<Move*> BattleState::moves(Entity entity) {
    return data<std::vector<Move*>>(entity, gameData);
}

PokemonSpeciesData& BattleState::species(Entity entity) {
    return data<PokemonSpeciesData>(entity, gameData);
}

void BattleState::loadDetailedPokemonData() {
    std::vector<Entity> pokemonList = {
        battle->playerPokemon,
        battle->opponentPokemon
    };

    for (const auto& pokemonEntity : pokemonList) {
        loadMoves(pokemonEntity);
        loadSpeciesData(pokemonEntity);
        addComponent(pokemonEntity, VolatileData{}, gameData);
    }
}

void BattleState::loadMoves(Entity pokemonEntity) {
    const Pokemon& currentPokemon = pokemon(pokemonEntity);
    std::vector<Move*> moveList;

    for (size_t i = 0; i < currentPokemon.moves.size(); ++i) {
        Move& move = resource<Move>("move-" + currentPokemon.moves[i], gameData);
        moveList.push_back(&move);
    }

    addComponent(pokemonEntity, moveList, gameData);
}

void BattleState::loadSpeciesData(Entity pokemonEntity) {
    const Pokemon& currentPokemon = pokemon(pokemonEntity);
    PokemonSpeciesData& speciesData = resource<PokemonSpeciesData>(
        "pokemon-" + currentPokemon.species,
        gameData
    );

    addComponent(pokemonEntity, speciesData, gameData);
}
