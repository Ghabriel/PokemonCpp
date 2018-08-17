#include "states/OverworldState.hpp"

#include <algorithm>
#include <cmath>
#include "components/DisabledControls.hpp"
#include "components/Camera.hpp"
#include "components/Map.hpp"
#include "components/Velocity.hpp"
#include "CoreStructures.hpp"
#include "core-functions.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/input-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"
#include "engine/utils/timing/print-fps.hpp"
#include "EventQueue.hpp"
#include "lua-native-functions.hpp"
#include "overworld/is-next-tile-blocked.hpp"
#include "overworld/on-tile-step.hpp"
#include "overworld/overworld-utils.hpp"
#include "overworld/process-interaction.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::entitysystem::ComponentManager;
using engine::entitysystem::Entity;
using engine::inputsystem::InputContext;
using engine::spritesystem::AnimationPlaybackData;
using engine::spritesystem::LoopingAnimationData;
using engine::spritesystem::playAnimations;

constexpr float tileProximityThreshold = 0.05;

bool isPositionNearInt(const Position& position, float threshold = tileProximityThreshold) {
    return
        (std::abs(std::round(position.x) - position.x) < threshold) &&
        (std::abs(std::round(position.y) - position.y) < threshold);
}

OverworldState::OverworldState(CoreStructures& gameData)
 : gameData(gameData),
   player(createEntity(gameData)),
   map(createEntity(gameData)) {
    addComponent(player, Direction::South, gameData);
    addComponent(player, Position{5, 5}, gameData);
    addComponent(player, Velocity{0, 0}, gameData);

    registerInputContext();
    updatePlayerAnimation(player, gameData);
    removeComponent<AnimationPlaybackData>(player, gameData);

    gameData.resourceStorage->store("player-event-queue", EventQueue());

    lua::internal::setCoreStructures(gameData);
    lua::internal::setMap(map);
    lua::internal::setPlayer(player);
}

void OverworldState::registerInputContext() {
    {
        InputContext context;

        context.actions = {
            {"Action", [&] { processInteraction(gameData, player, map); }},
            {"Cancel", [&] { std::cout << "Cancel\n"; }},
            {"Start", [&] { std::cout << "Start\n"; }}
        };

        context.states = {
            {"Left", [&] { onPressDirectionKey(Direction::West); }},
            {"Right", [&] { onPressDirectionKey(Direction::East); }},
            {"Up", [&] { onPressDirectionKey(Direction::North); }},
            {"Down", [&] { onPressDirectionKey(Direction::South); }}
        };

        context.priority = 0;

        gameData.inputDispatcher->registerContext("overworld-state", context);
    }
    {
        engine::inputsystem::InputContext context;

        context.actions = {
            {"Action", [] {}},
            {"Cancel", [] {}},
            {"Start", [] {}},
            {"Left", [] {}},
            {"Right", [] {}},
            {"Up", [] {}},
            {"Down", [] {}}
        };

        context.states = {
            {"Action", [] {}},
            {"Cancel", [] {}},
            {"Start", [] {}},
            {"Left", [] {}},
            {"Right", [] {}},
            {"Up", [] {}},
            {"Down", [] {}}
        };

        context.priority = 9;

        gameData.inputDispatcher->registerContext("disabled-controls", context);
    }
}

void OverworldState::onEnterImpl() {
    enableInputContext("overworld-state", gameData);
    music("bgm-littleroot-town", gameData).play();
    addComponent(player, AnimationPlaybackData{}, gameData);
    addComponent(map, resource<Map>("map-basic", gameData), gameData);
    restoreEntity(player, gameData);
    restoreEntity(map, gameData);
}

void OverworldState::onExitImpl() {
    disableInputContext("overworld-state", gameData);
    music("bgm-littleroot-town", gameData).pause();
    removeComponent<AnimationPlaybackData>(player, gameData);
    deleteEntity(player, gameData);
    deleteEntity(map, gameData);
}

void OverworldState::executeImpl() {
    engine::utils::printFPS<1>("Overworld Update Rate", 50);

    if (
        !hasComponent<DisabledControls>(player, gameData) &&
        isPlayerNearlyAlignedToTile() &&
        isMoving()
    ) {
        onNearTile();
    }

    resource<EventQueue>("player-event-queue", gameData).tick();
    processMovingEntities();
    adjustPlayerSpritePosition();
    adjustCameraPosition();

    playAnimations<LoopingAnimationData>(
        *gameData.componentManager,
        *gameData.timeSinceLastFrame
    );
    pressingDirectionKey = false;
}

void OverworldState::onNearTile() {
    Position& playerPosition = data<Position>(player, gameData);

    if (!pressingDirectionKey) {
        stopWalking();
        onTileChange();
        return;
    }

    float distanceToLastTile =
        (playerPosition.x - lastPlayerTile.x) * (playerPosition.x - lastPlayerTile.x) +
        (playerPosition.y - lastPlayerTile.y) * (playerPosition.y - lastPlayerTile.y);

    if (distanceToLastTile >= 1 - tileProximityThreshold) {
        onTileChange();
    }

    if (isMovingTowardsBlockedTile()) {
        stopWalking();
    }
}

void OverworldState::onTileChange() {
    // Position& playerPosition = data<Position>(player, gameData);
    // ECHO("onTileStep(" + std::to_string(playerPosition.x) + ", " + std::to_string(playerPosition.y) + ")");

    if (onTileStep(gameData, player, map)) {
        stopWalking();
    }

    lastPlayerTile = data<Position>(player, gameData);
}

bool OverworldState::isMovingTowardsBlockedTile() const {
    if (!isMoving()) {
        return false;
    }

    return isNextTileBlocked(gameData, player, map);
}

bool OverworldState::isMoving() const {
    Velocity& playerVelocity = data<Velocity>(player, gameData);
    return (playerVelocity.x != 0 || playerVelocity.y != 0);
}

void OverworldState::processMovingEntities() {
    ComponentManager& manager = *gameData.componentManager;
    double timeSinceLastFrame = *gameData.timeSinceLastFrame;

    manager.forEachEntity<Velocity, Position>(
        [&](
            Entity entity,
            Velocity& velocity,
            Position& position
        ) {
            position.x += velocity.x * timeSinceLastFrame;
            position.y += velocity.y * timeSinceLastFrame;
        }
    );
}

void OverworldState::adjustPlayerSpritePosition() {
    static const auto tileSize = settings(gameData).getTileSize();
    LoopingAnimationData& playerAnimation = data<LoopingAnimationData>(player, gameData);
    static const auto playerWidth = playerAnimation.frames[0].width;
    static const auto playerHeight = playerAnimation.frames[0].height;
    Position& playerPosition = data<Position>(player, gameData);

    playerAnimation.sprite.setPosition({
        (playerPosition.x + 0.5f) * tileSize - playerWidth / 2,
        (playerPosition.y + 0.75f) * tileSize - playerHeight
    });
}

void OverworldState::adjustCameraPosition() {
    LoopingAnimationData& playerAnimation = data<LoopingAnimationData>(player, gameData);
    static const auto playerHeight = playerAnimation.frames[0].height;
    auto& playerSpritePosition = playerAnimation.sprite.getPosition();

    Camera& camera = resource<Camera>("camera", gameData);
    camera.x = playerSpritePosition.x - camera.width / 2;
    camera.y = playerSpritePosition.y - camera.height / 2 + playerHeight / 2;

    Map& mapData = data<Map>(map, gameData);
    static const float tileSize = settings(gameData).getTileSize();
    camera.x = std::clamp(camera.x, 0.f, mapData.widthInTiles * tileSize - camera.width);
    camera.y = std::clamp(camera.y, 0.f, mapData.heightInTiles * tileSize - camera.height);
}

void OverworldState::onPressDirectionKey(Direction direction) {
    pressingDirectionKey = true;
    Direction& currentDirection = data<Direction>(player, gameData);

    if (direction == currentDirection) {
        if (!isMoving()) {
            startWalking();
        }

        return;
    }

    if (!isPlayerNearlyAlignedToTile()) {
        return;
    }

    currentDirection = direction;
    stopWalking();
}

void OverworldState::startWalking() {
    Direction& playerDirection = data<Direction>(player, gameData);
    std::pair<int, int> directionOffsets = getDirectionOffsets(playerDirection);
    static const auto speed = settings(gameData).getPlayerWalkingSpeed();
    data<Velocity>(player, gameData) = {
        speed * directionOffsets.first,
        speed * directionOffsets.second
    };
    updatePlayerAnimation(player, gameData);
}

void OverworldState::stopWalking() {
    alignPlayerToNearestTile();
    data<Velocity>(player, gameData) = {0, 0};
    updatePlayerAnimation(player, gameData);
}

bool OverworldState::isPlayerNearlyAlignedToTile() const {
    return isPositionNearInt(data<Position>(player, gameData));
}

void OverworldState::alignPlayerToNearestTile() {
    Position& currentPosition = data<Position>(player, gameData);
    currentPosition.x = std::round(currentPosition.x);
    currentPosition.y = std::round(currentPosition.y);
}
