#include "overworld/events/PlayerSpinningMoveEvent.hpp"

#include <cmath>
#include <unordered_map>
#include "components/Velocity.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "overworld/overworld-utils.hpp"

using engine::entitysystem::Entity;

const std::unordered_map<Direction, Direction> clockwiseMove = {
    {Direction::North, Direction::East},
    {Direction::East, Direction::South},
    {Direction::South, Direction::West},
    {Direction::West, Direction::North}
};

const std::unordered_map<Direction, Direction> antiClockwiseMove = {
    {Direction::North, Direction::West},
    {Direction::West, Direction::South},
    {Direction::South, Direction::East},
    {Direction::East, Direction::North}
};

PlayerSpinningMoveEvent::PlayerSpinningMoveEvent(
    Direction direction,
    int numTiles,
    int spinDelayMs,
    bool clockwise,
    Entity player,
    CoreStructures& gameData
) : direction(direction),
    numTiles(numTiles),
    spinDelayMs(spinDelayMs),
    clockwise(clockwise),
    player(player),
    gameData(gameData) { }

void PlayerSpinningMoveEvent::onStartImpl() {
    initialPosition = data<Position>(player, gameData);
    timeAccumulator = 0;
    std::pair<int, int> directionOffsets = getDirectionOffsets(direction);
    static const auto speed = settings(gameData).getPlayerWalkingSpeed();
    data<Velocity>(player, gameData) = {
        speed * directionOffsets.first,
        speed * directionOffsets.second
    };
    updatePlayerAnimation(player, gameData);
}

bool PlayerSpinningMoveEvent::tickImpl() {
    Position& playerPosition = data<Position>(player, gameData);
    float deltaX = playerPosition.x - initialPosition.x;
    float deltaY = playerPosition.y - initialPosition.y;
    float distanceSquared = deltaX * deltaX + deltaY * deltaY;

    if (std::abs(distanceSquared - numTiles * numTiles) < 0.8f) {
        data<Velocity>(player, gameData) = {0, 0};
        updatePlayerAnimation(player, gameData);
        return true;
    }

    timeAccumulator += *gameData.timeSinceLastFrame;
    if (timeAccumulator >= spinDelayMs) {
        Direction& playerDirection = data<Direction>(player, gameData);

        if (clockwise) {
            playerDirection = clockwiseMove.at(playerDirection);
        } else {
            playerDirection = antiClockwiseMove.at(playerDirection);
        }

        updatePlayerAnimation(player, gameData);
        timeAccumulator = 0;
    }

    return false;
}
