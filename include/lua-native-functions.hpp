#ifndef LUA_NATIVE_FUNCTIONS_HPP
#define LUA_NATIVE_FUNCTIONS_HPP

#include <string>
#include "battle/data/BoundMove.hpp"
#include "core-functions.hpp"
#include "engine/entity-system/types.hpp"
#include "engine/scripting-system/forward-declarations.hpp"

struct CoreStructures;

enum class Stat;
enum class StatFlags;

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

    // TODO: move LuaSupportedType-related things to a new file (lua-type-conversion?)
    template<typename T>
    struct LuaSupportedType {
        using type = T;
    };

    template<>
    struct LuaSupportedType<engine::entitysystem::Entity> {
        using type = std::unordered_map<std::string, int>;
    };

    template<>
    struct LuaSupportedType<StatFlags> {
        using type = int;
    };

    template<>
    struct LuaSupportedType<Stat> {
        using type = int;
    };

    template<>
    struct LuaSupportedType<BoundMove> {
        using type = std::unordered_map<std::string, int>;
    };

    template<typename T>
    inline typename LuaSupportedType<T>::type toLuaValue(const T& value) {
        return static_cast<typename LuaSupportedType<T>::type>(value);
    }

    template<>
    inline typename LuaSupportedType<engine::entitysystem::Entity>::type toLuaValue(
        const engine::entitysystem::Entity& value
    ) {
        return {
            {"id", static_cast<int>(value)}
        };
    }

    template<>
    inline typename LuaSupportedType<BoundMove>::type toLuaValue(const BoundMove& value) {
        return {
            {"pokemonId", value.user},
            {"moveIndex", value.moveIndex}
        };
    }

    template<typename Ret, typename... Args>
    Ret call(const std::string& functionName, Args&&... args) {
        CoreStructures& gameData = internal::detail::getCoreStructures();
        engine::scriptingsystem::Lua& luaFile = script("moves", gameData);
        return luaFile.call<Ret>(
            functionName,
            toLuaValue(std::forward<Args>(args))...
        );
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
