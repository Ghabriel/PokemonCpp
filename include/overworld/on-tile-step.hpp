#ifndef ON_TILE_STEP_HPP
#define ON_TILE_STEP_HPP

#include "../CoreStructures.hpp"
#include "../engine/entity-system/types.hpp"

bool onTileStep(
    CoreStructures& gameData,
    engine::entitysystem::Entity player,
    engine::entitysystem::Entity map
);

#endif
