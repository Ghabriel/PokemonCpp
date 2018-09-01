#ifndef SCRIPTING_SYSTEM_LUA_WRAPPER_HPP
#define SCRIPTING_SYSTEM_LUA_WRAPPER_HPP

#include <cassert>
#include <functional>
#include <utility>
#include "LuaRAII.hpp"
#include "../utils/debug/xtrace.hpp"

namespace engine::scriptingsystem {
    namespace __detail {
        template<typename Ret, typename... Args>
        using CFunction = Ret (*)(Args...);

        template<typename T>
        T getArgument(lua_State* L, int index);

        template<>
        inline bool getArgument(lua_State* L, int index) {
            return lua_toboolean(L, index);
        }

        template<>
        inline float getArgument(lua_State* L, int index) {
            assert(lua_isnumber(L, index));
            return lua_tonumber(L, index);
        }

        template<>
        inline int getArgument(lua_State* L, int index) {
            assert(lua_isnumber(L, index));
            return lua_tonumber(L, index);
        }

        template<>
        inline std::string getArgument(lua_State* L, int index) {
            assert(lua_isstring(L, index));
            return std::string(lua_tostring(L, index));
        }

        template<typename T>
        inline T get(lua_State* L) {
            return getArgument<T>(L, -1);
        }

        template<typename Ret, typename... Args, size_t... Is>
        inline void callWithArguments(
            CFunction<Ret, Args...> fn,
            const std::index_sequence<Is...>&,
            lua_State* rawState
        ) {
            fn(getArgument<std::decay_t<Args>>(rawState, Is + 1)...);
        }
    }

    class LuaWrapper {
        template<typename Ret, typename... Args>
        using CFunction = __detail::CFunction<Ret, Args...>;
     public:
        using LuaCFunction = CFunction<int, lua_State*>;

        LuaWrapper(const std::string& filename);

        void pushGlobal(const std::string& variableName);
        void pushField(const std::string& fieldName);
        void pop(size_t count = 1);
        template<typename T>
        void pushValue(const T&);
        template <typename Ret, typename... Args>
        void pushFunction(CFunction<Ret, Args...>);
        void setGlobal(const std::string& globalName);
        void setField(const std::string& fieldName);
        void call(size_t paramCount, size_t returnCount);
        void eval(const std::string& code);

        bool isNil() const;
        template<typename T>
        T get() const;

     private:
        LuaRAII L;
    };

    inline LuaWrapper::LuaWrapper(const std::string& filename) : L(filename) { }

    inline void LuaWrapper::pushGlobal(const std::string& variableName) {
        lua_getglobal(L.get(), variableName.c_str());
    }

    inline void LuaWrapper::pushField(const std::string& fieldName) {
        lua_getfield(L.get(), -1, fieldName.c_str());
    }

    inline void LuaWrapper::pop(size_t count) {
        lua_pop(L.get(), count);
    }

    template<>
    inline void LuaWrapper::pushValue(const bool& value) {
        lua_pushboolean(L.get(), value);
    }

    template<>
    inline void LuaWrapper::pushValue(const float& value) {
        lua_pushnumber(L.get(), value);
    }

    template<>
    inline void LuaWrapper::pushValue(const int& value) {
        lua_pushnumber(L.get(), value);
    }

    template<>
    inline void LuaWrapper::pushValue(const std::string& value) {
        lua_pushstring(L.get(), value.c_str());
    }

    template<typename Ret, typename... Args>
    void LuaWrapper::pushFunction(CFunction<Ret, Args...> fn) {
        struct Container {
            static int luaWrapper(lua_State* rawState) {
                void* voidFn = lua_touserdata(rawState, lua_upvalueindex(1));
                auto retrievedFn = reinterpret_cast<CFunction<Ret, Args...>>(voidFn);
                __detail::callWithArguments(
                    retrievedFn,
                    std::make_index_sequence<sizeof...(Args)>{},
                    rawState
                );

                if constexpr (std::is_same_v<Ret, void>) {
                    return 0;
                } else {
                    return 1;
                }
            }
        };

        lua_pushlightuserdata(L.get(), reinterpret_cast<void*>(fn));
        lua_pushcclosure(L.get(), &Container::luaWrapper, 1);
    }

    template<>
    inline void LuaWrapper::pushFunction(LuaCFunction fn) {
        lua_pushcfunction(L.get(), fn);
    }

    inline void LuaWrapper::setGlobal(const std::string& globalName) {
        lua_setglobal(L.get(), globalName.c_str());
    }

    inline void LuaWrapper::setField(const std::string& fieldName) {
        lua_setfield(L.get(), -2, fieldName.c_str());
    }

    inline void LuaWrapper::call(size_t paramCount, size_t returnCount) {
        lua_pcall(L.get(), paramCount, returnCount, 0);
    }

    inline void LuaWrapper::eval(const std::string& code) {
        luaL_dostring(L.get(), code.c_str());
    }

    inline bool LuaWrapper::isNil() const {
        return lua_isnil(L.get(), -1);
    }

    template<typename T>
    inline T LuaWrapper::get() const {
        return __detail::get<T>(L.get());
    }
}

#endif
