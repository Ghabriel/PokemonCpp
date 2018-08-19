#ifndef LUA_NATIVE_FUNCTIONS_HPP
#define LUA_NATIVE_FUNCTIONS_HPP

#include <string>
#include "engine/entity-system/types.hpp"
#include "engine/scripting-system/forward-declarations.hpp"

struct CoreStructures;

namespace lua {
    namespace internal {
        void setCoreStructures(CoreStructures&);
        void setBattle(engine::entitysystem::Entity);
        void setMap(engine::entitysystem::Entity);
        void setPlayer(engine::entitysystem::Entity);
    }

    // Generic events
    void log(const std::string& str);
    void disableControls();
    void enableControls();
    void showText(const std::string& content);
    void wait(int ms);

    // Overworld events
    void movePlayerNorth(int numTiles);
    void movePlayerWest(int numTiles);
    void movePlayerEast(int numTiles);
    void movePlayerSouth(int numTiles);
    void moveSpinningPlayerNorth(int numTiles, int spinDelayMs, bool clockwise);
    void moveSpinningPlayerWest(int numTiles, int spinDelayMs, bool clockwise);
    void moveSpinningPlayerEast(int numTiles, int spinDelayMs, bool clockwise);
    void moveSpinningPlayerSouth(int numTiles, int spinDelayMs, bool clockwise);
    void turnPlayerNorth();
    void turnPlayerWest();
    void turnPlayerEast();
    void turnPlayerSouth();

    // Battle-related events
    void possibleWildBattle();
    void showBattleText(const std::string& content);
}

void injectNativeFunctions(engine::scriptingsystem::Lua& script);

#endif
