#include "overworld/overworld-utils.hpp"

#include <cmath>
#include <string>
#include "components/Direction.hpp"
#include "components/Map.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "core-functions.hpp"
#include "engine/sfml/sprite-system/include.hpp"

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

    int targetTileX = std::round(playerPosition.x + tileOffsetX);
    int targetTileY = std::round(playerPosition.y + tileOffsetY);
    return std::make_pair(targetTileX, targetTileY);
}

Lua& getMapScripts(Entity map, CoreStructures& gameData) {
    Map& mapData = data<Map>(map, gameData);
    return script("map-" + std::to_string(mapData.id), gameData);
}

void updatePlayerAnimation(Entity player, CoreStructures& gameData) {
    Velocity& currentVelocity = data<Velocity>(player, gameData);
    bool isMoving = (currentVelocity.x != 0 || currentVelocity.y != 0);

    std::string animationId;
    switch (data<Direction>(player, gameData)) {
        case Direction::North:
            animationId = isMoving ? "player-walking-north" : "player-facing-north";
            break;
        case Direction::West:
            animationId = isMoving ? "player-walking-west" : "player-facing-west";
            break;
        case Direction::East:
            animationId = isMoving ? "player-walking-east" : "player-facing-east";
            break;
        case Direction::South:
            animationId = isMoving ? "player-walking-south" : "player-facing-south";
            break;
    }

    using engine::spritesystem::AnimationPlaybackData;
    using engine::spritesystem::LoopingAnimationData;
    removeComponent<AnimationPlaybackData>(player, gameData);
    removeComponent<LoopingAnimationData>(player, gameData);
    auto& animation = resource<LoopingAnimationData>(animationId, gameData);
    addComponent(player, animation, gameData);
    addComponent(player, AnimationPlaybackData{}, gameData);
}
