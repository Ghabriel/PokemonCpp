#include "lua-native-functions.hpp"

#include <iostream>
#include "components/Direction.hpp"
#include "components/DisabledControls.hpp"
#include "components/Velocity.hpp"
#include "CoreStructures.hpp"
#include "core-functions.hpp"
#include "engine/scripting-system/include.hpp"
#include "events/PlayerMoveEvent.hpp"
#include "overworld/overworld-utils.hpp"

#include "EventQueue.hpp"
#include "engine/utils/debug/xtrace.hpp"

CoreStructures* gameData;
engine::entitysystem::Entity player;

void lua::internal::setCoreStructures(CoreStructures& _gameData) {
    gameData = &_gameData;
}

void lua::internal::setPlayer(engine::entitysystem::Entity _player) {
    player = _player;
}

void lua::write(const std::string& str) {
    std::cout << str << std::endl;
}

void lua::disableControls() {
    addComponent(player, DisabledControls{}, *gameData);
    enableInputContext("disabled-controls", *gameData);
}

void lua::enableControls() {
    removeComponent<DisabledControls>(player, *gameData);
    disableInputContext("disabled-controls", *gameData);
}

void lua::movePlayerSouth(int numTiles) {
    // std::cout << "moveSouth(" << numTiles << ")" << std::endl;
    // data<Direction>(player, *gameData) = Direction::South;
    // updatePlayerAnimation(player, *gameData);

    EventQueue& queue = resource<EventQueue>("player-event-queue", *gameData);
    queue.addEvent(std::make_unique<PlayerMoveEvent>(
        Direction::South,
        numTiles,
        player,
        *gameData
    ));
}

void injectNativeFunctions(engine::scriptingsystem::Lua& script) {
    script.registerNative("write", lua::write);
    script.registerNative("disableControls", lua::disableControls);
    script.registerNative("enableControls", lua::enableControls);
    script.registerNative("movePlayerSouth", lua::movePlayerSouth);
}
