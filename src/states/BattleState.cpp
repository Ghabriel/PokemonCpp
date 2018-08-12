#include "states/BattleState.hpp"

#include "core-functions.hpp"
#include "CoreStructures.hpp"

using engine::inputsystem::InputContext;

BattleState::BattleState(CoreStructures& gameData) : gameData(gameData) {
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
    sf::Music& bgm = music("bgm-wild-battle", gameData);
    // bgm.setLoopPoints(4, bgm.getDuration() - sf::seconds(6));
    bgm.setLoop(true);
    bgm.play();
    // bgm.setPlayingOffset(sf::seconds(170));
}

void BattleState::onExitImpl() {
    disableInputContext("battle-state", gameData);
    music("bgm-wild-battle", gameData).stop();
}

void BattleState::executeImpl() {
    // TODO
}
