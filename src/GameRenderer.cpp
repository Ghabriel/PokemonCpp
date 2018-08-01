#include "GameRenderer.hpp"

#include <iostream>
#include "engine/entity-system/include.hpp"
#include "engine/game-loop/SingleThreadGameLoop.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/utils/timing/print-fps.hpp"

GameRenderer::GameRenderer(ComponentManager& manager, ResourceStorage& storage)
 : windowPtr(new sf::RenderWindow(sf::VideoMode(800, 600), "Pokemon")),
   componentManager(manager),
   resourceStorage(storage) { }

void GameRenderer::operator()(SingleThreadGameLoop& game) {
    engine::utils::printFPS<2>("FPS", 2000);

    sf::RenderWindow& window = *windowPtr;

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            window.close();
            game.stop();
        } else if (event.type == sf::Event::Resized) {
            sf::Vector2u windowSize = window.getSize();
            double width = windowSize.x;
            double height = windowSize.y;
            view = sf::View(sf::FloatRect(0, 0, width, height));
            window.setView(view);
        }
    }

    window.clear();
    // TODO: render the game objects
    window.display();
}
