#ifndef SCRIPTING_SYSTEM_LUA_RAII_HPP
#define SCRIPTING_SYSTEM_LUA_RAII_HPP

#include <stdexcept>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

namespace engine::scriptingsystem {
    class LuaRAII {
     public:
        LuaRAII(const std::string& filename);
        ~LuaRAII();

        constexpr lua_State* get() const {
            return L;
        }

     private:
        lua_State* L;
    };

    LuaRAII::LuaRAII(const std::string& filename) {
        L = luaL_newstate();

        if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0)) {
            // TODO: doesn't this cause a memory leak?
            L = nullptr;
            throw std::runtime_error("Failed to open script: " + filename);
        }
    }

    LuaRAII::~LuaRAII() {
        if (L) {
            lua_close(L);
        }
    }
}

#endif
