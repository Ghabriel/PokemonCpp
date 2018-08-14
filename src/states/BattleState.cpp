#include "states/BattleState.hpp"

#include "battle/battle-setup.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "EventQueue.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::inputsystem::InputContext;

template<typename TEvent, typename... Args>
void enqueueEvent(CoreStructures& gameData, Args&&... args) {
    EventQueue& queue = resource<EventQueue>("battle-event-queue", gameData);
    queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
}

BattleState::BattleState(CoreStructures& gameData)
 : gameData(gameData),
   battle(createEntity(gameData)) {
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
    setupWildEncounter("map-basic", battle, gameData);
    gameData.resourceStorage->store("battle-event-queue", EventQueue());
}

void BattleState::onExitImpl() {
    disableInputContext("battle-state", gameData);
    music("bgm-wild-battle", gameData).stop();
}

void BattleState::executeImpl() {
    // TODO
}
