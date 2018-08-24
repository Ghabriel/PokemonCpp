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

namespace {
    enum class BattleAction {
        Fight,
        Bag,
        Pokemon,
        Run
    };

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
        gameData
    );

    enqueueEvent<ImmediateEvent>(gameData, [&] {
        removeComponent<BattleActionSelection>(battleEntity, gameData);

        switch (static_cast<BattleAction>(selectedAction)) {
            case BattleAction::Fight:
                // TODO: use Struggle if no moves can be used
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
        false,
        [&]() -> size_t& {
            return data<BattleMoveSelection>(battleEntity, gameData).focusedOption;
        },
        gameData
    );

    enqueueEvent<ImmediateEvent>(gameData, [&] {
        removeComponent<BattleMoveSelection>(battleEntity, gameData);
        // TODO: handle lack of PP
        processTurn();
    });
}

void BattleState::processTurn() {
    enqueueEvent<ImmediateEvent>(gameData, [&] {
        Pokemon& playerPokemon = pokemon(battle->playerPokemon);
        const Move& playerMove = *moves(battle->playerPokemon)[selectedAction];
        int playerSpeed = playerPokemon.stats[5];

        Pokemon& opponentPokemon = pokemon(battle->opponentPokemon);
        size_t opponentMoveIndex = chooseMoveAI(opponentPokemon);
        const Move& opponentMove = *moves(battle->opponentPokemon)[opponentMoveIndex];
        int opponentSpeed = opponentPokemon.stats[5];

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
    std::unordered_map<std::string, Pokemon*> pokemonList = {
        {"user", &pokemon(battle->opponentPokemon)},
        {"foe", &pokemon(battle->playerPokemon)},
    };

    auto& ai = script("ai", gameData);

    for (const auto& [varName, pokemonPtr] : pokemonList) {
        const auto& pokemon = *pokemonPtr;
        ai.set(varName + ".species", pokemon.species);
        ai.set(varName + ".nature", static_cast<int>(pokemon.nature));
        ai.set(varName + ".heldItem", pokemon.heldItem);
        ai.set(varName + ".ability", pokemon.ability);

        for (size_t i = 0; i < constants::MOVE_LIMIT; ++i) {
            if (i < pokemon.moves.size()) {
                ai.set(varName + ".move" + std::to_string(i), pokemon.moves[i]);
                ai.set(varName + ".pp" + std::to_string(i), pokemon.pp[i]);
            } else {
                ai.set(varName + ".move" + std::to_string(i), std::string{});
                ai.set(varName + ".pp" + std::to_string(i), std::string{});
            }
        }

        ai.set<int>(varName + ".moveCount", pokemon.moves.size());

        ai.set(varName + ".gender", static_cast<int>(pokemon.gender));
        ai.set(varName + ".form", pokemon.form);
        ai.set(varName + ".displayName", pokemon.displayName);
        ai.set(varName + ".status", static_cast<int>(pokemon.status));
        ai.set(varName + ".asleepRounds", pokemon.asleepRounds);
        ai.set(varName + ".level", pokemon.level);
        ai.set(varName + ".hp", pokemon.stats[0]);
        ai.set(varName + ".attack", pokemon.stats[1]);
        ai.set(varName + ".defense", pokemon.stats[2]);
        ai.set(varName + ".specialAttack", pokemon.stats[3]);
        ai.set(varName + ".specialDefense", pokemon.stats[4]);
        ai.set(varName + ".speed", pokemon.stats[5]);
        ai.set(varName + ".currentHP", pokemon.currentHP);
    }
}

size_t BattleState::chooseMoveAI(const Pokemon& pokemon) {
    updateAIVariables();
    return script("ai", gameData).call<int>("chooseMoveWildBattle");
}

void BattleState::processPlayerMove(size_t moveIndex) {
    enqueueEvent<ImmediateEvent>(gameData, [&, moveIndex] {
        Pokemon& user = pokemon(battle->playerPokemon);

        if (user.currentHP <= 0) {
            return;
        }

        Move& move = *moves(battle->playerPokemon)[moveIndex];
        showMoveText(user.displayName + " used " + move.displayName + "!");
        // TODO: move animation?
        // TODO: handle non-single-target moves
        processMove(&user, &pokemon(battle->opponentPokemon), &move);
    });
}

void BattleState::processOpponentMove(size_t moveIndex) {
    enqueueEvent<ImmediateEvent>(gameData, [&, moveIndex] {
        Pokemon& user = pokemon(battle->opponentPokemon);

        if (user.currentHP <= 0) {
            return;
        }

        Move& move = *moves(battle->opponentPokemon)[moveIndex];
        showMoveText("Foe " + user.displayName + " used " + move.displayName + "!");
        // TODO: move animation?
        // TODO: handle non-single-target moves
        processMove(&user, &pokemon(battle->playerPokemon), &move);
    });
}

void BattleState::processMove(Pokemon* user, Pokemon* target, Move* move) {
    effects::internal::setMoveUser(*user);
    effects::internal::setMoveTarget(*target);
    effects::internal::setMove(*move);

    switch (move->functionCode) {
        case 0:
            effects::damage();
            break;
        case -1:
        case -2:
        case -3:
            effects::lowerStat(move->functionParameter, -move->functionCode);
            break;
        case 1:
        case 2:
        case 3:
            effects::raiseStat(move->functionParameter, move->functionCode);
            break;
        case 99:
            script("moves", gameData).call<void>(move->id + "_onUse");
            break;
    }

    checkFaintedPokemon();
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
                    showMoveText("You blacked out!");
                    // TODO: move the player somewhere safe (in the Overworld)
                    enqueueMoveEvent<ImmediateEvent>(gameData, [&] {
                        gameData.stateMachine->pushState("overworld-state");
                    });
                } else {
                    rewardScreen();
                }
            }
        }
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
        const Pokemon& currentPokemon = pokemon(pokemonEntity);
        std::vector<Move*> moveList;

        for (size_t i = 0; i < currentPokemon.moves.size(); ++i) {
            Move& move = resource<Move>("move-" + currentPokemon.moves[i], gameData);
            moveList.push_back(&move);
        }

        addComponent(pokemonEntity, moveList, gameData);

        PokemonSpeciesData& speciesData = resource<PokemonSpeciesData>(
            "pokemon-" + currentPokemon.species,
            gameData
        );

        addComponent(pokemonEntity, speciesData, gameData);
    }
}
