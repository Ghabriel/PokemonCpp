#include "GameRenderer.hpp"

#include <iostream>
#include "engine/entity-system/include.hpp"
#include "engine/game-loop/SingleThreadGameLoop.hpp"
#include "engine/resource-system/include.hpp"

GameRenderer::GameRenderer(ComponentManager& manager, ResourceStorage& storage)
 : componentManager(manager), resourceStorage(storage) { }

void GameRenderer::operator()(SingleThreadGameLoop&) {
    // TODO
}
