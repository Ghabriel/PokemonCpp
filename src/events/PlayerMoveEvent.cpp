#include "events/PlayerMoveEvent.hpp"

#include <cmath>
#include "components/Velocity.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "overworld/overworld-utils.hpp"

#include "lua-native-functions.hpp"
#include "engine/utils/debug/xtrace.hpp"

using engine::entitysystem::Entity;

PlayerMoveEvent::PlayerMoveEvent(
    Direction direction,
    int numTiles,
    Entity player,
    CoreStructures& gameData
) : direction(direction), numTiles(numTiles), player(player), gameData(gameData) { }

void PlayerMoveEvent::onStartImpl() {
    initialPosition = data<Position>(player, gameData);
    data<Direction>(player, gameData) = direction;
    data<Velocity>(player, gameData) = {0, 0.005};
    updatePlayerAnimation(player, gameData);
}

bool PlayerMoveEvent::tickImpl() {
    Position& playerPosition = data<Position>(player, gameData);
    float deltaX = playerPosition.x - initialPosition.x;
    float deltaY = playerPosition.y - initialPosition.y;
    float distanceSquared = deltaX * deltaX + deltaY * deltaY;

    if (std::abs(distanceSquared - numTiles * numTiles) < 0.8f) {
        data<Velocity>(player, gameData) = {0, 0};
        updatePlayerAnimation(player, gameData);
        lua::enableControls();
        return true;
    }

    return false;
}
