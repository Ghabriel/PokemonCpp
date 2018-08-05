#include "states/OverworldState.hpp"

#include <cmath>
#include "components/Camera.hpp"
#include "components/Map.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "CoreStructures.hpp"
#include "core-functions.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/input-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"
#include "engine/utils/timing/print-fps.hpp"
#include "overworld/processInteraction.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::entitysystem::ComponentManager;
using engine::entitysystem::Entity;
using engine::inputsystem::InputContext;
using engine::spritesystem::AnimationPlaybackData;
using engine::spritesystem::LoopingAnimationData;
using engine::spritesystem::playAnimations;

bool isPositionNearInt(const Position& position, float threshold = 0.05f) {
    return
        (std::abs(std::round(position.x) - position.x) < threshold) &&
        (std::abs(std::round(position.y) - position.y) < threshold);
}

OverworldState::OverworldState(CoreStructures& gameData)
 : gameData(gameData),
   player(createEntity(gameData)) {
    addComponent(player, Direction::South, gameData);
    addComponent(player, Position{5, 5}, gameData);
    addComponent(player, Velocity{0, 0}, gameData);

    registerInputContext();
    updatePlayerAnimation();
    removeComponent<AnimationPlaybackData>(player, gameData);
}

void OverworldState::registerInputContext() {
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

    gameData.inputDispatcher->registerContext("overworld-state", context);
}

void OverworldState::onEnterImpl() {
    enableInputContext("overworld-state", gameData);
    sf::Music& bgm = music("bgm-littleroot-town", gameData);
    bgm.setLoop(true);
    bgm.play();

    addComponent(player, AnimationPlaybackData{}, gameData);

    addComponent(map, resource<Map>("map-basic", gameData), gameData);
}

void OverworldState::onExitImpl() {
    disableInputContext("overworld-state", gameData);
    music("bgm-littleroot-town", gameData).pause();
    removeComponent<AnimationPlaybackData>(player, gameData);
}

void OverworldState::executeImpl() {
    engine::utils::printFPS<1>("Overworld Update Rate", 50);

    if (!pressingDirectionKey || movingTowardsBlockedTile()) {
        stopWalking();
    }

    processMovingEntities();
    adjustPlayerSpritePosition();
    adjustCameraPosition();

    playAnimations<LoopingAnimationData>(
        *gameData.componentManager,
        *gameData.timeSinceLastFrame
    );
    pressingDirectionKey = false;
}

bool OverworldState::movingTowardsBlockedTile() const {
    Velocity& playerVelocity = data<Velocity>(player, gameData);
    if (playerVelocity.x == 0 && playerVelocity.y == 0) {
        return false;
    }

    return isNextTileBlocked(gameData, player, map);
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
}

void OverworldState::onPressDirectionKey(Direction direction) {
    pressingDirectionKey = true;
    Direction& currentDirection = data<Direction>(player, gameData);

    if (direction == currentDirection) {
        Velocity& currentVelocity = data<Velocity>(player, gameData);

        if (currentVelocity.x == 0 && currentVelocity.y == 0) {
            startWalking();
        }

        return;
    }

    if (!isPlayerNearlyAlignedToTile()) {
        return;
    }

    data<Direction>(player, gameData) = direction;
    onChangePlayerDirection();
}

void OverworldState::startWalking() {
    static const auto speed = settings(gameData).getPlayerWalkingSpeed();

    Velocity velocity;
    switch (data<Direction>(player, gameData)) {
        case Direction::North:
            velocity = Velocity{0, -speed};
            break;
        case Direction::West:
            velocity = Velocity{-speed, 0};
            break;
        case Direction::East:
            velocity = Velocity{speed, 0};
            break;
        case Direction::South:
            velocity = Velocity{0, speed};
            break;
    }

    data<Velocity>(player, gameData) = velocity;
    updatePlayerAnimation();
}

void OverworldState::stopWalking() {
    Velocity& playerVelocity = data<Velocity>(player, gameData);
    if ((playerVelocity.x != 0 || playerVelocity.y != 0) && isPlayerNearlyAlignedToTile()) {
        onChangePlayerDirection();
    }
}

void OverworldState::onChangePlayerDirection() {
    alignPlayerToNearestTile();
    data<Velocity>(player, gameData) = {0, 0};
    updatePlayerAnimation();
}

void OverworldState::updatePlayerAnimation() {
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

    removeComponent<AnimationPlaybackData>(player, gameData);
    removeComponent<LoopingAnimationData>(player, gameData);
    auto& animation = resource<LoopingAnimationData>(animationId, gameData);
    addComponent(player, animation, gameData);
    addComponent(player, AnimationPlaybackData{}, gameData);
}

bool OverworldState::isPlayerNearlyAlignedToTile() const {
    return isPositionNearInt(data<Position>(player, gameData));
}

void OverworldState::alignPlayerToNearestTile() {
    Position& currentPosition = data<Position>(player, gameData);
    currentPosition.x = std::round(currentPosition.x);
    currentPosition.y = std::round(currentPosition.y);
}
