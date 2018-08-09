// #include <SFML/Graphics.hpp>
// #include <X11/Xlib.h>
#include "components/Camera.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/game-loop/SingleThreadGameLoop.hpp"
#include "engine/resource-system/include.hpp"
#include "GameLogic.hpp"
#include "GameRenderer.hpp"
#include "Settings.hpp"

int main(int argc, char** argv) {
    // XInitThreads();
    engine::entitysystem::ComponentManager componentManager;
    engine::resourcesystem::ResourceStorage resourceStorage;

    resourceStorage.store("settings", Settings{});
    Settings& settings = resourceStorage.get<Settings>("settings");
    resourceStorage.store("camera", Camera{
        0,
        0,
        settings.getInitialWindowWidth(),
        settings.getInitialWindowHeight()
    });

    GameLogic logic(componentManager, resourceStorage);
    GameRenderer renderer(componentManager, resourceStorage);

    engine::gameloop::SingleThreadGameLoop gameLoop(
        [&](auto& gameLoop, double timeSinceLastFrame) { logic(gameLoop, timeSinceLastFrame); },
        [&](auto& gameLoop) { renderer(gameLoop); }
    );
    gameLoop.setUpdateFrequency(60);

    gameLoop.start();
    gameLoop.join();
}
