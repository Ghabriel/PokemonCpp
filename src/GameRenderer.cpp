#include "GameRenderer.hpp"

#include "components/Camera.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/game-loop/SingleThreadGameLoop.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/utils/timing/print-fps.hpp"
#include "render.hpp"
#include "Settings.hpp"

#include "engine/utils/debug/xtrace.hpp"

GameRenderer::GameRenderer(ComponentManager& manager, ResourceStorage& storage)
 : componentManager(manager),
   resourceStorage(storage) {
    Camera& camera = storage.get<Camera>("camera");
    Settings& settings = storage.get<Settings>("settings");

    windowPtr = std::make_unique<sf::RenderWindow>(
        sf::VideoMode(camera.width, camera.height),
        settings.getWindowTitle()
    );

    adjustView();
}

void GameRenderer::operator()(SingleThreadGameLoop& game) {
    engine::utils::printFPS<2>("FPS", 2000);
    // static Settings& settings = resourceStorage.get<Settings>("settings");
    // static int minWindowWidth = settings.getMinWindowWidth();
    // static int minWindowHeight = settings.getMinWindowHeight();

    adjustView();

    sf::RenderWindow& window = *windowPtr;

    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed) {
            game.stop();
        } else if (event.type == sf::Event::Resized) {
            sf::Vector2u windowSize = window.getSize();
            // XTRACE(windowSize.x);
            // XTRACE(windowSize.y);
            // XTRACE(minWindowWidth);
            // XTRACE(minWindowHeight);
            // bool adjustedSize = false;

            // if (windowSize.x < minWindowWidth) {
            //     windowSize.x = minWindowWidth;
            //     adjustedSize = true;
            // }

            // if (windowSize.y < minWindowHeight) {
            //     windowSize.y = minWindowHeight;
            //     adjustedSize = true;
            // }

            // if (adjustedSize) {
            //     window.setSize(windowSize);
            //     ECHO("[CHANGE]");
            //     XTRACE(windowSize.x);
            //     XTRACE(windowSize.y);
            //     XTRACE(minWindowWidth);
            //     XTRACE(minWindowHeight);
            //     ECHO("------------");
            // }

            Camera& camera = resourceStorage.get<Camera>("camera");
            camera.width = windowSize.x;
            camera.height = windowSize.y;
            adjustView();
        }
    }

    window.clear();
    render(window, componentManager, resourceStorage);
    window.display();
}

void GameRenderer::adjustView() {
    Camera& camera = resourceStorage.get<Camera>("camera");
    view.reset(sf::FloatRect(camera.x, camera.y, camera.width, camera.height));
    windowPtr->setView(view);
}
