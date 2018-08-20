#include "lua-native-functions.hpp"

#include <iostream>
#include "components/Direction.hpp"
#include "components/DisabledControls.hpp"
#include "components/Velocity.hpp"
#include "CoreStructures.hpp"
#include "core-functions.hpp"
#include "engine/scripting-system/include.hpp"
#include "engine/state-system/include.hpp"
#include "EventQueue.hpp"
#include "events/ImmediateEvent.hpp"
#include "events/ScreenFadeEvent.hpp"
#include "events/TextEvent.hpp"
#include "events/WaitEvent.hpp"
#include "overworld/events/PlayerMoveEvent.hpp"
#include "overworld/events/PlayerSpinningMoveEvent.hpp"
#include "overworld/overworld-utils.hpp"

#include "engine/utils/debug/xtrace.hpp"

namespace {
    CoreStructures* gameData;
    engine::entitysystem::Entity battle;
    engine::entitysystem::Entity map;
    engine::entitysystem::Entity player;

    template<typename TEvent, typename... Args>
    void enqueueEvent(Args&&... args) {
        EventQueue& queue = resource<EventQueue>("player-event-queue", *gameData);
        queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
    }

    template<typename TEvent, typename... Args>
    void enqueueBattleEvent(Args&&... args) {
        EventQueue& queue = resource<EventQueue>("battle-event-queue", *gameData);
        queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
    }

    void movePlayer(Direction direction, int numTiles) {
        enqueueEvent<PlayerMoveEvent>(direction, numTiles, player, *gameData);
    }

    void moveSpinningPlayer(
        Direction direction,
        int numTiles,
        int spinDelayMs,
        bool clockwise
    ) {
        enqueueEvent<PlayerSpinningMoveEvent>(
            direction,
            numTiles,
            spinDelayMs,
            clockwise,
            player,
            *gameData
        );
    }

    void turnPlayer(Direction direction) {
        enqueueEvent<ImmediateEvent>([&, direction] {
            data<Direction>(player, *gameData) = direction;
            updatePlayerAnimation(player, *gameData);
        });
    }
}


void lua::internal::setCoreStructures(CoreStructures& _gameData) {
    gameData = &_gameData;
}

void lua::internal::setBattle(engine::entitysystem::Entity _battle) {
    battle = _battle;
}

void lua::internal::setMap(engine::entitysystem::Entity _map) {
    map = _map;
}

void lua::internal::setPlayer(engine::entitysystem::Entity _player) {
    player = _player;
}

void lua::log(const std::string& str) {
    std::cout << str << std::endl;
}

// Generic events
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

void lua::showText(const std::string& content) {
    enqueueEvent<TextEvent>(content, map, *gameData);
}

void lua::wait(int ms) {
    enqueueEvent<WaitEvent>(ms, *gameData);
}

// Overworld events
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

void lua::moveSpinningPlayerNorth(int numTiles, int spinDelayMs, bool clockwise) {
    moveSpinningPlayer(Direction::North, numTiles, spinDelayMs, clockwise);
}

void lua::moveSpinningPlayerWest(int numTiles, int spinDelayMs, bool clockwise) {
    moveSpinningPlayer(Direction::West, numTiles, spinDelayMs, clockwise);
}

void lua::moveSpinningPlayerEast(int numTiles, int spinDelayMs, bool clockwise) {
    moveSpinningPlayer(Direction::East, numTiles, spinDelayMs, clockwise);
}

void lua::moveSpinningPlayerSouth(int numTiles, int spinDelayMs, bool clockwise) {
    moveSpinningPlayer(Direction::South, numTiles, spinDelayMs, clockwise);
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

// Battle-related events
void lua::possibleWildBattle() {
    // TODO: check if this will really happen
    disableControls();
    enqueueEvent<ScreenFadeEvent>(map, *gameData);
    enqueueEvent<ImmediateEvent>([&] {
        gameData->stateMachine->pushState("battle-state");
    });
}

void lua::showBattleText(const std::string& content) {
    ECHO("showBattleText");
    XTRACE(content);
    XTRACE(battle);
    enqueueBattleEvent<TextEvent>(content, battle, *gameData);
}


void injectNativeFunctions(engine::scriptingsystem::Lua& script) {
    script.registerNative("log", lua::log);
    script.registerNative("disableControls", lua::disableControls);
    script.registerNative("enableControls", lua::enableControls);
    script.registerNative("showText", lua::showText);
    script.registerNative("wait", lua::wait);

    script.registerNative("movePlayerNorth", lua::movePlayerNorth);
    script.registerNative("movePlayerWest", lua::movePlayerWest);
    script.registerNative("movePlayerEast", lua::movePlayerEast);
    script.registerNative("movePlayerSouth", lua::movePlayerSouth);
    script.registerNative("moveSpinningPlayerNorth", lua::moveSpinningPlayerNorth);
    script.registerNative("moveSpinningPlayerWest", lua::moveSpinningPlayerWest);
    script.registerNative("moveSpinningPlayerEast", lua::moveSpinningPlayerEast);
    script.registerNative("moveSpinningPlayerSouth", lua::moveSpinningPlayerSouth);
    script.registerNative("turnPlayerNorth", lua::turnPlayerNorth);
    script.registerNative("turnPlayerWest", lua::turnPlayerWest);
    script.registerNative("turnPlayerEast", lua::turnPlayerEast);
    script.registerNative("turnPlayerSouth", lua::turnPlayerSouth);

    script.registerNative("possibleWildBattle", lua::possibleWildBattle);
    script.registerNative("showBattleText", lua::showBattleText);
}
