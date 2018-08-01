#include "GameLogic.hpp"

#include <iostream>
#include "engine/entity-system/include.hpp"
#include "engine/game-loop/SingleThreadGameLoop.hpp"
#include "engine/resource-system/include.hpp"

GameLogic::GameLogic(ComponentManager& manager, ResourceStorage& storage)
 : componentManager(manager), resourceStorage(storage) { }

void GameLogic::operator()(SingleThreadGameLoop&, double timeSinceLastFrame) {
    // TODO
    std::cout << "GameLogic::operator()\n";
}
