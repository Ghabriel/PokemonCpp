#include "states/OverworldState.hpp"

#include <cmath>
#include "components/Map.hpp"
#include "components/Position.hpp"
#include "components/Velocity.hpp"
#include "CoreStructures.hpp"
#include "core-functions.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/input-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"
#include "engine/utils/timing/print-fps.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::entitysystem::ComponentManager;
using engine::entitysystem::Entity;
using engine::inputsystem::InputContext;
using engine::spritesystem::AnimationPlaybackData;
using engine::spritesystem::LoopingAnimationData;
using engine::spritesystem::playAnimations;

bool isPositionNearInt(const Position& position, float threshold = 0.1f) {
    return
        (std::abs(std::round(position.x) - position.x) < threshold) &&
        (std::abs(std::round(position.y) - position.y) < threshold);
}

OverworldState::OverworldState(CoreStructures& gameData)
 : gameData(gameData),
   player(createEntity(gameData)) {
    addComponent(player, Direction::South, gameData);
    addComponent(player, Position{0, 2}, gameData);
    addComponent(player, Velocity{0, 0}, gameData);

    registerInputContext();
    updatePlayerAnimation();
    removeComponent<AnimationPlaybackData>(player, gameData);
}

void OverworldState::executeImpl() {
    static const auto tileSize = settings(gameData).getTileSize();
    double timeSinceLastFrame = *gameData.timeSinceLastFrame;
    engine::utils::printFPS<1>("Overworld Update Rate", 50);

    ComponentManager& manager = *gameData.componentManager;
    Position& playerPosition = manager.getData<Position>(player);
    LoopingAnimationData& playerAnimation = manager.getData<LoopingAnimationData>(player);
    static const auto playerWidth = playerAnimation.frames[0].width;
    static const auto playerHeight = playerAnimation.frames[0].height;

    playerAnimation.sprite.setPosition({
        (playerPosition.x + 0.5f) * tileSize - playerWidth / 2,
        (playerPosition.y + 0.5f) * tileSize - playerHeight
    });

    playAnimations<LoopingAnimationData>(manager, timeSinceLastFrame);

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

void OverworldState::onEnterImpl() {
    enableInputContext("overworld-state", gameData);
    sf::Music& bgm = music("bgm-littleroot-town", gameData);
    bgm.setLoop(true);
    bgm.play();

    addComponent(player, AnimationPlaybackData{}, gameData);

    auto& basicMap = resource<Map>("map-basic", gameData);
    addComponent(map, basicMap, gameData);
}

void OverworldState::onExitImpl() {
    disableInputContext("overworld-state", gameData);
    music("bgm-littleroot-town", gameData).pause();
    removeComponent<AnimationPlaybackData>(player, gameData);
}

void OverworldState::registerInputContext() {
    InputContext context;

    context.actions = {
        {"Action", [&] { std::cout << "Action\n"; }},
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

void OverworldState::onPressDirectionKey(Direction direction) {
    Direction& currentDirection = data<Direction>(player, gameData);
    Velocity& currentVelocity = data<Velocity>(player, gameData);

    if (direction == currentDirection) {
        if (currentVelocity.x == 0 && currentVelocity.y == 0) {
            startWalking();
        }

        return;
    }

    Position& currentPosition = data<Position>(player, gameData);

    if (!isPositionNearInt(currentPosition)) {
        // too far from tile alignment, ignore
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

void OverworldState::onChangePlayerDirection() {
    Position& currentPosition = data<Position>(player, gameData);
    currentPosition.x = std::round(currentPosition.x);
    currentPosition.y = std::round(currentPosition.y);
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
