#ifndef CORE_STRUCTURES_HPP
#define CORE_STRUCTURES_HPP

#include "engine/entity-system/forward-declarations.hpp"
#include "engine/input-system/forward-declarations.hpp"
#include "engine/resource-system/forward-declarations.hpp"
#include "engine/state-system/forward-declarations.hpp"

struct CoreStructures {
    engine::entitysystem::ComponentManager* componentManager;
    engine::inputsystem::InputDispatcher* inputDispatcher;
    engine::resourcesystem::ResourceStorage* resourceStorage;
    engine::statesystem::StateMachine* stateMachine;
    double* timeSinceLastFrame;
};

#endif
