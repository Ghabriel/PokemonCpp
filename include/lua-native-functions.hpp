#ifndef LUA_NATIVE_FUNCTIONS_HPP
#define LUA_NATIVE_FUNCTIONS_HPP

#include <string>
#include "engine/entity-system/types.hpp"
#include "engine/scripting-system/forward-declarations.hpp"

struct CoreStructures;

namespace lua {
    namespace internal {
        void setCoreStructures(CoreStructures&);
        void setPlayer(engine::entitysystem::Entity);
    }

    void write(const std::string& str);
    void disableControls();
    void enableControls();
    void movePlayerNorth(int numTiles);
    void movePlayerWest(int numTiles);
    void movePlayerEast(int numTiles);
    void movePlayerSouth(int numTiles);
    void turnPlayerNorth();
    void turnPlayerWest();
    void turnPlayerEast();
    void turnPlayerSouth();
    void wait(int ms);
}

void injectNativeFunctions(engine::scriptingsystem::Lua& script);

#endif
