#include "states/OverworldState.hpp"

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

OverworldState::OverworldState(CoreStructures& gameData)
 : gameData(gameData),
   player(createEntity(gameData)) {
    addComponent(player, Direction::South, gameData);
    addComponent(player, Position{0, 2}, gameData);

    registerInputContext();
    onChangePlayerDirection();
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

    auto& basicMap = gameData.resourceStorage->get<Map>("map-basic");
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
        {"Start", [&] { std::cout << "Start\n"; addComponent(player, Velocity{0.005, 0}, gameData); }}
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

    if (direction == currentDirection) {
        walk();
        return;
    }

    currentDirection = direction;
    onChangePlayerDirection();
}

void OverworldState::walk() {
    constexpr float speed = 0.005;
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

    removeComponent<Velocity>(player, gameData);
    addComponent(player, velocity, gameData);
}

void OverworldState::onChangePlayerDirection() {
    removeComponent<LoopingAnimationData>(player, gameData);

    std::string animationId;
    switch (data<Direction>(player, gameData)) {
        case Direction::North:
            animationId = "player-walking-north";
            break;
        case Direction::West:
            animationId = "player-walking-west";
            break;
        case Direction::East:
            animationId = "player-walking-east";
            break;
        case Direction::South:
            animationId = "player-walking-south";
            break;
    }

    auto& animation = gameData.resourceStorage->get<LoopingAnimationData>(animationId);
    addComponent(player, animation, gameData);
}
