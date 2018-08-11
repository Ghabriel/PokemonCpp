#ifndef LUA_NATIVE_FUNCTIONS_HPP
#define LUA_NATIVE_FUNCTIONS_HPP

#include <string>
#include "engine/entity-system/types.hpp"
#include "engine/scripting-system/forward-declarations.hpp"

struct CoreStructures;

namespace lua {
    namespace internal {
        void setCoreStructures(CoreStructures&);
        void setMap(engine::entitysystem::Entity);
        void setPlayer(engine::entitysystem::Entity);
    }

    void write(const std::string& str);
    void disableControls();
    void enableControls();
    void movePlayerNorth(int numTiles);
    void movePlayerWest(int numTiles);
    void movePlayerEast(int numTiles);
    void movePlayerSouth(int numTiles);
    void moveSpinningPlayerNorth(int numTiles, int spinDelayMs, bool clockwise);
    void moveSpinningPlayerWest(int numTiles, int spinDelayMs, bool clockwise);
    void moveSpinningPlayerEast(int numTiles, int spinDelayMs, bool clockwise);
    void moveSpinningPlayerSouth(int numTiles, int spinDelayMs, bool clockwise);
    void showText(const std::string& content);
    void turnPlayerNorth();
    void turnPlayerWest();
    void turnPlayerEast();
    void turnPlayerSouth();
    void wait(int ms);
}

void injectNativeFunctions(engine::scriptingsystem::Lua& script);

#endif
