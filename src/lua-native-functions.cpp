#include "lua-native-functions.hpp"

#include <iostream>
#include "components/Direction.hpp"
#include "components/DisabledControls.hpp"
#include "components/Velocity.hpp"
#include "CoreStructures.hpp"
#include "core-functions.hpp"
#include "engine/scripting-system/include.hpp"
#include "events/ImmediateEvent.hpp"
#include "events/PlayerMoveEvent.hpp"
#include "events/WaitEvent.hpp"
#include "overworld/overworld-utils.hpp"

#include "EventQueue.hpp"
#include "engine/utils/debug/xtrace.hpp"

CoreStructures* gameData;
engine::entitysystem::Entity player;

template<typename TEvent, typename... Args>
void enqueueEvent(Args&&... args) {
    EventQueue& queue = resource<EventQueue>("player-event-queue", *gameData);
    queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
}

void movePlayer(Direction direction, int numTiles) {
    enqueueEvent<PlayerMoveEvent>(direction, numTiles, player, *gameData);
}

void turnPlayer(Direction direction) {
    enqueueEvent<ImmediateEvent>([&, direction] {
        data<Direction>(player, *gameData) = direction;
        updatePlayerAnimation(player, *gameData);
    });
}


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
    enqueueEvent<ImmediateEvent>([&] {
        addComponent(player, DisabledControls{}, *gameData);
        enableInputContext("disabled-controls", *gameData);
    });
}

void lua::enableControls() {
    enqueueEvent<ImmediateEvent>([&] {
        removeComponent<DisabledControls>(player, *gameData);
        disableInputContext("disabled-controls", *gameData);
    });
}

void lua::movePlayerNorth(int numTiles) {
    movePlayer(Direction::North, numTiles);
}

void lua::movePlayerWest(int numTiles) {
    movePlayer(Direction::West, numTiles);
}

void lua::movePlayerEast(int numTiles) {
    movePlayer(Direction::East, numTiles);
}

void lua::movePlayerSouth(int numTiles) {
    movePlayer(Direction::South, numTiles);
}

void lua::turnPlayerNorth() {
    turnPlayer(Direction::North);
}

void lua::turnPlayerWest() {
    turnPlayer(Direction::West);
}

void lua::turnPlayerEast() {
    turnPlayer(Direction::East);
}

void lua::turnPlayerSouth() {
    turnPlayer(Direction::South);
}


void lua::wait(int ms) {
    enqueueEvent<WaitEvent>(ms, *gameData);
}

void injectNativeFunctions(engine::scriptingsystem::Lua& script) {
    script.registerNative("write", lua::write);
    script.registerNative("disableControls", lua::disableControls);
    script.registerNative("enableControls", lua::enableControls);
    script.registerNative("movePlayerNorth", lua::movePlayerNorth);
    script.registerNative("movePlayerWest", lua::movePlayerWest);
    script.registerNative("movePlayerEast", lua::movePlayerEast);
    script.registerNative("movePlayerSouth", lua::movePlayerSouth);
    script.registerNative("turnPlayerNorth", lua::turnPlayerNorth);
    script.registerNative("turnPlayerWest", lua::turnPlayerWest);
    script.registerNative("turnPlayerEast", lua::turnPlayerEast);
    script.registerNative("turnPlayerSouth", lua::turnPlayerSouth);
    script.registerNative("wait", lua::wait);
}
