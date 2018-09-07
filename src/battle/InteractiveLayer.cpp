#include "battle/InteractiveLayer.hpp"

#include <cassert>
#include "battle/BattleController.hpp"
#include "battle/data/Move.hpp"
#include "battle/data/Pokemon.hpp"
#include "battle/data/PokemonSpeciesData.hpp"
#include "battle/events/ActionSelectionEvent.hpp"
#include "battle/helpers/battle-utils.hpp"
#include "components/battle/Battle.hpp"
#include "components/battle/BattleActionSelection.hpp"
#include "components/battle/BattleMoveSelection.hpp"
#include "core-functions.hpp"
#include "engine/state-system/include.hpp"
#include "events/ImmediateEvent.hpp"
#include "events/TextEvent.hpp"
#include "EventQueue.hpp"

namespace {
    enum class BattleAction {
        Fight,
        Bag,
        Pokemon,
        Run
    };

    constexpr int STRUGGLE = -2;
    constexpr auto INTERACTION_QUEUE = "battle-interaction-event-queue";

    template<typename TEvent, typename... Args>
    void enqueueEvent(CoreStructures& gameData, Args&&... args) {
        EventQueue& queue = resource<EventQueue>(INTERACTION_QUEUE, gameData);
        queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
    }
}

InteractiveLayer::InteractiveLayer(
    Entity battleEntity,
    BattleController& controller,
    CoreStructures& gameData
) : battleEntity(battleEntity),
    battleController(&controller),
    gameData(&gameData) { }

void InteractiveLayer::startBattle() {
    battleController->startBattle();
    battle = &data<Battle>(battleEntity, *gameData);

    gameData->resourceStorage->store(INTERACTION_QUEUE, EventQueue());

    std::string& displayName = data<Pokemon>(battle->opponentTeam[0], *gameData).displayName;
    showText("Wild " + displayName + " appeared!");
    // TODO: only show the player's pokémon now
    actionSelectionScreen();
}

void InteractiveLayer::abort() {
    battleController->abort();
    gameData->resourceStorage->remove<EventQueue>(INTERACTION_QUEUE);
}

void InteractiveLayer::tick() {
    if (battleController->hasPendingEvents()) {
        battleController->tick();
        return;
    }

    resource<EventQueue>(INTERACTION_QUEUE, *gameData).tick();
}

void InteractiveLayer::actionSelectionScreen() {
    enqueueEvent<ImmediateEvent>(*gameData, [&] {
        addComponent(
            battleEntity,
            BattleActionSelection{
                "What will " + pokemon(battle->playerTeam[0]).displayName + " do?",
                {"Fight", "Bag", "Pokemon", "Run"},
                300,
                0
            },
            *gameData
        );
    });

    enqueueEvent<ActionSelectionEvent>(
        *gameData,
        selectedAction,
        false,
        [&]() -> size_t& {
            return data<BattleActionSelection>(battleEntity, *gameData).focusedOption;
        },
        [](size_t) { return true; },
        *gameData
    );

    enqueueEvent<ImmediateEvent>(*gameData, [&] {
        removeComponent<BattleActionSelection>(battleEntity, *gameData);

        switch (static_cast<BattleAction>(selectedAction)) {
            case BattleAction::Fight:
                if (!hasUsableMoves(battle->playerTeam[0], *gameData)) {
                    std::string& displayName = pokemon(battle->playerTeam[0]).displayName;
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
                sound("fx-battle-run", *gameData).play();
                showText("Got away safely!");
                enqueueEvent<ImmediateEvent>(*gameData, [&] {
                    gameData->stateMachine->pushState("overworld-state");
                });
                break;
        }
    });
}

void InteractiveLayer::moveSelectionScreen() {
    enqueueEvent<ImmediateEvent>(*gameData, [&] {
        BattleMoveSelection moveSelection;
        const Pokemon& currentPokemon = pokemon(battle->playerTeam[0]);
        const auto& moveList = moves(battle->playerTeam[0]);
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
        addComponent(battleEntity, moveSelection, *gameData);
    });

    enqueueEvent<ActionSelectionEvent>(
        *gameData,
        selectedAction,
        true,
        [&]() -> size_t& {
            return data<BattleMoveSelection>(battleEntity, *gameData).focusedOption;
        },
        [&](size_t option) {
            return option < pokemon(battle->playerTeam[0]).moves.size();
        },
        *gameData
    );

    enqueueEvent<ImmediateEvent>(*gameData, [&] {
        removeComponent<BattleMoveSelection>(battleEntity, *gameData);

        if (selectedAction == -1) {
            actionSelectionScreen();
            return;
        }

        if (pokemon(battle->playerTeam[0]).pp[selectedAction] == 0) {
            showText("That move has no PP left!"); // TODO
            moveSelectionScreen();
            return;
        }

        processTurn();
    });
}

void InteractiveLayer::processTurn() {
    enqueueEvent<ImmediateEvent>(*gameData, [&] {
        BoundMove playerMove = getUsedMoveBy(
            battle->playerTeam[0],
            battle->opponentTeam[0],
            selectedAction
        );

        int opponentMoveIndex = battleController->chooseMoveAI(
            pokemon(battle->opponentTeam[0])
        );
        BoundMove opponentMove = getUsedMoveBy(
            battle->opponentTeam[0],
            battle->playerTeam[0],
            opponentMoveIndex
        );

        battleController->processTurn({playerMove, opponentMove});

        enqueueEvent<ImmediateEvent>(*gameData, [this] {
            switch (battleController->getState()) {
                case BattleController::State::PENDING_START:
                    assert(false);
                    break;
                case BattleController::State::READY:
                    actionSelectionScreen();
                    break;
                case BattleController::State::VICTORY:
                    rewardScreen();
                    break;
                case BattleController::State::DEFEAT:
                case BattleController::State::DRAW:
                    blackOutScreen();
                    break;
            }
        });
    });
}

void InteractiveLayer::blackOutScreen() {
    showText("You blacked out!");
    // TODO: move the player somewhere safe (in the Overworld)
    enqueueEvent<ImmediateEvent>(*gameData, [&] {
        gameData->stateMachine->pushState("overworld-state");
    });
}

void InteractiveLayer::rewardScreen() {
    DEBUG enqueueEvent<ImmediateEvent>(*gameData, [&] {
        music("bgm-wild-battle", *gameData).stop();
        music("bgm-wild-battle-victory", *gameData).play();
    });

    DEBUG Pokemon& playerPokemon = pokemon(battle->playerTeam[0]);
    DEBUG int exp = calculateExpGain(
        playerPokemon,
        pokemon(battle->opponentTeam[0]),
        data<PokemonSpeciesData>(battle->opponentTeam[0], *gameData)
    );
    DEBUG showText(playerPokemon.displayName + " gained " + std::to_string(exp) + " EXP. Points!");

    DEBUG enqueueEvent<ImmediateEvent>(*gameData, [&] {
        deleteEntity(battle->opponentTeam[0], *gameData);
        music("bgm-wild-battle-victory", *gameData).stop();
        gameData->stateMachine->pushState("overworld-state");
    });
}

BoundMove InteractiveLayer::getUsedMoveBy(Entity user, Entity target, int selectedAction) {
    Move& move = (selectedAction == STRUGGLE)
        ? resource<Move>("move-Struggle", *gameData)
        : *moves(user)[selectedAction];

    return {user, target, &move, selectedAction};
}

Pokemon& InteractiveLayer::pokemon(Entity entity) {
    return data<Pokemon>(entity, *gameData);
}

std::vector<Move*> InteractiveLayer::moves(Entity entity) {
    return data<std::vector<Move*>>(entity, *gameData);
}

void InteractiveLayer::showText(const std::string& content) {
    enqueueEvent<TextEvent>(*gameData, content, battleEntity, *gameData);
}
