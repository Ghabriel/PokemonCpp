#ifndef LUA_NATIVE_FUNCTIONS_HPP
#define LUA_NATIVE_FUNCTIONS_HPP

#include <string>
#include "core-functions.hpp"
#include "engine/entity-system/types.hpp"
#include "engine/scripting-system/forward-declarations.hpp"

struct CoreStructures;

namespace lua {
    namespace internal {
        void setCoreStructures(CoreStructures&);
        void setBattle(engine::entitysystem::Entity);
        void setMap(engine::entitysystem::Entity);
        void setPlayer(engine::entitysystem::Entity);

        namespace detail {
            CoreStructures& getCoreStructures();
        }
    }

    // Generic events
    void log(const std::string& str);
    void disableControls();
    void enableControls();
    void showText(const std::string& content);
    void wait(int ms);

    template<typename Ret, typename... Args>
    Ret call(const std::string& functionName, Args&&... args) {
        CoreStructures& gameData = internal::detail::getCoreStructures();
        return script("moves", gameData).call<Ret>(functionName, args...);
    }

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
}

void injectNativeFunctions(engine::scriptingsystem::Lua& script);

#endif
