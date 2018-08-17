#include "states/BattleState.hpp"

#include "battle/battle-setup.hpp"
#include "battle/events/ActionSelectionEvent.hpp"
#include "components/Battle.hpp"
#include "components/BattleActionSelection.hpp"
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
    enqueueEvent<ActionSelectionEvent>(
        gameData,
        selectedAction,
        battle->playerPokemon,
        battleEntity,
        gameData
    );

    enqueueEvent<ImmediateEvent>(gameData, [&] {
        switch (static_cast<BattleAction>(selectedAction)) {
            case BattleAction::Fight:
                showText("TODO: fight");
                actionSelectionScreen();
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

void BattleState::showText(const std::string& content) {
    enqueueEvent<TextEvent>(gameData, content, battleEntity, gameData);
}
