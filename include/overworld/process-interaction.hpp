#ifndef PROCESS_INTERACTION_HPP
#define PROCESS_INTERACTION_HPP

#include "../CoreStructures.hpp"
#include "../engine/entity-system/types.hpp"

void processInteraction(
    CoreStructures& gameData,
    engine::entitysystem::Entity player,
    engine::entitysystem::Entity map
);

#endif
