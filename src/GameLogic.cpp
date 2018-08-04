#include "GameLogic.hpp"

#include <iostream>
#include "engine/entity-system/include.hpp"
#include "engine/game-loop/SingleThreadGameLoop.hpp"
#include "engine/resource-system/include.hpp"
#include "init/load-input-tracker.hpp"
#include "init/load-resources.hpp"
#include "init/register-states.hpp"
#include "ResourceFiles.hpp"
#include "Settings.hpp"

GameLogic::GameLogic(ComponentManager& manager, ResourceStorage& storage)
 : componentManager(manager),
   resourceStorage(storage),
   inputTracker(loadInputTracker(ResourceFiles::CONTROLS)),
   inputDispatcher(inputTracker) {
    gameData = {
        &componentManager,
        &inputDispatcher,
        &resourceStorage,
        &stateMachine,
        nullptr
    };

    Settings& settings = resourceStorage.get<Settings>("settings");

    loadResources(resourceStorage);
    registerStates(gameData);
    stateMachine.pushState(settings.getInitialState());
}

void GameLogic::operator()(SingleThreadGameLoop&, double timeSinceLastFrame) {
    gameData.timeSinceLastFrame = &timeSinceLastFrame;
    inputDispatcher.tick();
    stateMachine.execute();
}
