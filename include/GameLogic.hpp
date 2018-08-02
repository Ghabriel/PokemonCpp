#ifndef GAME_LOGIC_HPP
#define GAME_LOGIC_HPP

#include "engine/entity-system/forward-declarations.hpp"
#include "engine/game-loop/forward-declarations.hpp"
#include "engine/input-system/include.hpp"
#include "engine/resource-system/forward-declarations.hpp"
#include "engine/state-system/include.hpp"
#include "CoreStructures.hpp"

class GameLogic {
    using ComponentManager = engine::entitysystem::ComponentManager;
    using InputDispatcher = engine::inputsystem::InputDispatcher;
    using InputTracker = engine::inputsystem::InputTracker;
    using ResourceStorage = engine::resourcesystem::ResourceStorage;
    using SingleThreadGameLoop = engine::gameloop::SingleThreadGameLoop;
    using StateMachine = engine::statesystem::StateMachine;
  public:
    GameLogic(ComponentManager&, ResourceStorage&);
    void operator()(SingleThreadGameLoop&, double timeSinceLastFrame);

 private:
    ComponentManager& componentManager;
    ResourceStorage& resourceStorage;
    InputTracker inputTracker;
    InputDispatcher inputDispatcher;
    StateMachine stateMachine;
    CoreStructures gameData;
};

#endif
