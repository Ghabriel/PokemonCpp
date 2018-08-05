#include "overworld/overworld-utils.hpp"

#include <string>
#include "components/Direction.hpp"
#include "components/Map.hpp"
#include "components/Position.hpp"
#include "core-functions.hpp"

using engine::entitysystem::Entity;
using engine::scriptingsystem::Lua;

std::pair<int, int> getTargetTile(Entity player, CoreStructures& gameData) {
    Position& playerPosition = data<Position>(player, gameData);
    int tileOffsetX = 0;
    int tileOffsetY = 0;

    switch (data<Direction>(player, gameData)) {
        case Direction::North:
            tileOffsetY = -1;
            break;
        case Direction::West:
            tileOffsetX = -1;
            break;
        case Direction::East:
            tileOffsetX = 1;
            break;
        case Direction::South:
            tileOffsetY = 1;
            break;
    }

    int targetTileX = playerPosition.x + tileOffsetX;
    int targetTileY = playerPosition.y + tileOffsetY;
    return std::make_pair(targetTileX, targetTileY);
}

Lua& getMapScripts(Entity map, CoreStructures& gameData) {
    Map& mapData = data<Map>(map, gameData);
    return script("map-" + std::to_string(mapData.id), gameData);
}
