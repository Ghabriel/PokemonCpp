#include "states/BattleState.hpp"

#include "battle/battle-setup.hpp"
#include "battle/battle-utils.hpp"
#include "battle/events/ActionSelectionEvent.hpp"
#include "battle/Move.hpp"
#include "battle/Pokemon.hpp"
#include "battle/PokemonSpeciesData.hpp"
#include "battle/random.hpp"
#include "components/Battle.hpp"
#include "components/BattleActionSelection.hpp"
#include "components/BattleMoveSelection.hpp"
#include "components/TextBox.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "events/ImmediateEvent.hpp"
#include "events/TextEvent.hpp"
#include "EventQueue.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::inputsystem::InputContext;

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

PokemonSpeciesData& getSpecies(const Pokemon& pokemon, CoreStructures& gameData) {
    return resource<PokemonSpeciesData>("pokemon-" + pokemon.species, gameData);
}

BattleState::BattleState(CoreStructures& gameData)
 : gameData(gameData),
   battleEntity(createEntity(gameData)) {
    registerInputContext();
}

void BattleState::registerInputContext() {
    InputContext context;

    context.actions = {
        {"Action", [&] { std::cout << "Actionl\n"; }},
        {"Cancel", [&] { std::cout << "Cancel\n"; }},
        {"Start", [&] { std::cout << "Start\n"; }},
        {"Left", [&] { std::cout << "Arrow\n";/* onPressDirectionKey(Direction::West); */ }},
        {"Right", [&] { std::cout << "Arrow\n";/* onPressDirectionKey(Direction::East); */ }},
        {"Up", [&] { std::cout << "Arrow\n";/* onPressDirectionKey(Direction::North); */ }},
        {"Down", [&] { std::cout << "Arrow\n";/* onPressDirectionKey(Direction::South); */ }}
    };

    context.priority = 0;

    gameData.inputDispatcher->registerContext("battle-state", context);
}

void BattleState::onEnterImpl() {
    restoreEntity(battleEntity, gameData);
    enableInputContext("battle-state", gameData);
    setupWildEncounter("map-basic", battleEntity, gameData);
    gameData.resourceStorage->store("battle-event-queue", EventQueue());
    battle = &data<Battle>(battleEntity, gameData);

    showText("Wild " + battle->opponentPokemon.displayName + " appeared!");
    // TODO: only show the player's pokémon now
    actionSelectionScreen();
}

void BattleState::onExitImpl() {
    disableInputContext("battle-state", gameData);
    music("bgm-wild-battle", gameData).stop();
    deleteEntity(battleEntity, gameData);
}

void BattleState::executeImpl() {
    resource<EventQueue>("battle-event-queue", gameData).tick();
}

void BattleState::actionSelectionScreen() {
    enqueueEvent<ImmediateEvent>(gameData, [&] {
        addComponent(
            battleEntity,
            BattleActionSelection{
                "What will " + battle->playerPokemon.displayName + " do?",
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
        for (size_t i = 0; i < battle->playerPokemon.moves.size(); ++i) {
            const auto& pokemon = battle->playerPokemon;
            const auto& move = resource<Move>("move-" + pokemon.moves[i], gameData);
            int maxPP = move.pp * (1 + 0.2 * pokemon.ppUps[i]);

            moveSelection.moves[i] = {{
                move.displayName,
                move.type, // TODO: display name of type?
                pokemon.pp[i],
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
        Pokemon& playerPokemon = battle->playerPokemon;
        const auto& playerMoveId = playerPokemon.moves[selectedAction];
        const Move& playerMove = resource<Move>("move-" + playerMoveId, gameData);
        int playerSpeed = playerPokemon.stats[5];

        size_t opponentMoveIndex = chooseMoveAI(battle->opponentPokemon);
        Pokemon& opponentPokemon = battle->opponentPokemon;
        const auto& opponentMoveId = opponentPokemon.moves[opponentMoveIndex];
        const Move& opponentMove = resource<Move>("move-" + opponentMoveId, gameData);
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

        // actionSelectionScreen();
    });
}

size_t BattleState::chooseMoveAI(const Pokemon& pokemon) {
    // TODO: handle proper AIs
    return random(0, pokemon.moves.size() - 1);
}

void BattleState::processPlayerMove(size_t moveIndex) {
    enqueueEvent<ImmediateEvent>(gameData, [&, moveIndex] {
        Pokemon& user = battle->playerPokemon;
        const auto& moveId = user.moves[moveIndex];
        Move& move = resource<Move>("move-" + moveId, gameData);
        showText(user.displayName + " used " + move.displayName + "!");
        // TODO: move animation?
        // TODO: handle non-single-target moves
        processMove(&user, &battle->opponentPokemon, &move);
    });
}

void BattleState::processOpponentMove(size_t moveIndex) {
    enqueueEvent<ImmediateEvent>(gameData, [&, moveIndex] {
        Pokemon& user = battle->opponentPokemon;
        const auto& moveId = user.moves[moveIndex];
        Move& move = resource<Move>("move-" + moveId, gameData);
        showText("Foe " + user.displayName + " used " + move.displayName + "!");
        // TODO: move animation?
        // TODO: handle non-single-target moves
        processMove(&user, &battle->playerPokemon, &move);
    });
}

void BattleState::processMove(Pokemon* user, Pokemon* target, Move* move) {
    enqueueEvent<ImmediateEvent>(gameData, [&, user, target, move] {
        // TODO: handle non-damage moves
        PokemonSpeciesData& targetSpecies = getSpecies(*target, gameData);
        float type = getTypeEffectiveness(targetSpecies, *move);

        if (type < 0.1) {
            showText("It doesn't affect " + target->displayName + "...");
            return;
        }

        PokemonSpeciesData& userSpecies = getSpecies(*user, gameData);
        int attack = getAttackStatForMove(*user, *move);
        int defense = getDefenseStatForMove(*target, *move);
        int baseDamage = (((2 * user->level) / 5 + 2) * move->power * (attack / defense)) / 50 + 2;
        float targets = 1; // TODO: handle multi-target moves
        float weather = 1; // TODO
        float critical = 1; // TODO
        float rand = random(217, 255) / 255.0;
        float stab = (move->type == userSpecies.types[0] || move->type == userSpecies.types[1])
            ? 1.5
            : 1; // TODO: handle Adaptability
        float burn = 1; // TODO
        float others = 1; // TODO
        float modifier = targets * weather * critical * rand * stab * type * burn * others;
        int damage = baseDamage * modifier;

        if (damage == 0) {
            damage = 1;
        }

        target->currentHP -= damage;
    });
}

void BattleState::showText(const std::string& content) {
    enqueueEvent<TextEvent>(gameData, content, battleEntity, gameData);
}
