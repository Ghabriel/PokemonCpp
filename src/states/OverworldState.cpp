#include "states/OverworldState.hpp"

#include "components/Map.hpp"
#include "components/Position.hpp"
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
    auto& animation = gameData.resourceStorage->get<LoopingAnimationData>("player-walking-east");
    addComponent(player, animation, gameData);
    addComponent(player, Position{2, 2}, gameData);

    registerInputContext();
}

void OverworldState::executeImpl() {
    static const auto tileSize = settings(gameData).getTileSize();
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

    playAnimations<LoopingAnimationData>(manager, *gameData.timeSinceLastFrame);
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
        {"Start", [&] { std::cout << "Start\n"; }},
        {"Left", [&] { std::cout << "Left\n"; }},
        {"Right", [&] { std::cout << "Right\n"; }},
        {"Up", [&] { std::cout << "Up\n"; }},
        {"Down", [&] { std::cout << "Down\n"; }}
    };

    gameData.inputDispatcher->registerContext("overworld-state", context);
}
