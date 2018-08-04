#ifndef GAME_RENDERER_HPP
#define GAME_RENDERER_HPP

#include <SFML/Graphics.hpp>
#include <memory>
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
    std::unique_ptr<sf::RenderWindow> windowPtr;
    sf::View view;
    ComponentManager& componentManager;
    ResourceStorage& resourceStorage;

    void adjustView();
};

#endif
