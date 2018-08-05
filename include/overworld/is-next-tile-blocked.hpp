#ifndef IS_NEXT_TILE_BLOCKED_HPP
#define IS_NEXT_TILE_BLOCKED_HPP

#include "../CoreStructures.hpp"
#include "../engine/entity-system/types.hpp"

bool isNextTileBlocked(
    CoreStructures& gameData,
    engine::entitysystem::Entity player,
    engine::entitysystem::Entity map
);

#endif
