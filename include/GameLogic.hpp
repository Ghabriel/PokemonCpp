#ifndef GAME_LOGIC_HPP
#define GAME_LOGIC_HPP

#include "engine/entity-system/forward-declarations.hpp"
#include "engine/game-loop/forward-declarations.hpp"
#include "engine/resource-system/forward-declarations.hpp"

class GameLogic {
    using ComponentManager = engine::entitysystem::ComponentManager;
    using ResourceStorage = engine::resourcesystem::ResourceStorage;
    using SingleThreadGameLoop = engine::gameloop::SingleThreadGameLoop;
 public:
    GameLogic(ComponentManager&, ResourceStorage&);
    void operator()(SingleThreadGameLoop&, double timeSinceLastFrame);

 private:
    ComponentManager& componentManager;
    ResourceStorage& resourceStorage;
};

#endif
