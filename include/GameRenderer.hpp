#ifndef GAME_RENDERER_HPP
#define GAME_RENDERER_HPP

#include "engine/entity-system/forward-declarations.hpp"
#include "engine/game-loop/forward-declarations.hpp"
#include "engine/resource-system/forward-declarations.hpp"

class GameRenderer {
    using ComponentManager = engine::entitysystem::ComponentManager;
    using ResourceStorage = engine::resourcesystem::ResourceStorage;
    using SingleThreadGameLoop = engine::gameloop::SingleThreadGameLoop;
 public:
    GameRenderer(ComponentManager&, ResourceStorage&);
    void operator()(SingleThreadGameLoop&);

 private:
    ComponentManager& componentManager;
    ResourceStorage& resourceStorage;
};

#endif
