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
        LuaRAII(const LuaRAII&) = delete;
        LuaRAII(LuaRAII&&);
        ~LuaRAII();

        LuaRAII& operator=(const LuaRAII&) = delete;
        LuaRAII& operator=(LuaRAII&&);

        constexpr lua_State* get() const {
            return L;
        }

     private:
        lua_State* L;
    };

    inline LuaRAII::LuaRAII(const std::string& filename) {
        L = luaL_newstate();

        if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0)) {
            // TODO: doesn't this cause a memory leak?
            L = nullptr;
            throw std::runtime_error("Failed to open script: " + filename);
        }
    }

    inline LuaRAII::LuaRAII(LuaRAII&& other) {
        L = other.L;
        other.L = nullptr;
    }

    inline LuaRAII::~LuaRAII() {
        if (L) {
            lua_close(L);
        }
    }

    inline LuaRAII& LuaRAII::operator=(LuaRAII&& other) {
        L = other.L;
        other.L = nullptr;
        return *this;
    }
}

#endif
