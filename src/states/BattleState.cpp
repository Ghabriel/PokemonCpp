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
    bgm.setLoop(true);
    float loopStart = 16.8;
    float loopEnd = 175;
    bgm.setLoopPoints({sf::seconds(loopStart), sf::seconds(loopEnd - loopStart)});
    bgm.play();
}

void BattleState::onExitImpl() {
    disableInputContext("battle-state", gameData);
    music("bgm-wild-battle", gameData).stop();
}

void BattleState::executeImpl() {
    // TODO
}
