#ifndef SCRIPTING_SYSTEM_LUA_HPP
#define SCRIPTING_SYSTEM_LUA_HPP

#include <cassert>
#include <iostream>
#include <vector>
#include "LuaWrapper.hpp"

namespace engine::scriptingsystem {
    /**
     * \brief Provides a simple API to a %Lua file.
     */
    class Lua {
     public:
        Lua(const std::string& filename);

        /**
         * \brief Sets the value of a variable. The syntax "a.b.c" is
         * supported to set specific fields of a structure.
         */
        template<typename T>
        void set(const std::string& variableName, const T& value);

        /**
         * \brief Runs the given Lua code.
         */
        void eval(const std::string& code);

        /**
         * \brief Retrieves the value of a variable. The syntax "a.b.c" is
         * supported to retrieve specific fields of a structure.
         */
        template<typename T>
        T get(const std::string& variableName);

        /**
         * \brief Calls a %Lua function, given its name and an argument list.
         * The return type (T) must be specified.
         */
        template<typename T, typename... Args>
        T call(const std::string& functionName, Args&&...);

        /**
         * \brief Registers a C/C++ function to make it available in the %Lua
         * script with a specified name.
         */
        template<typename Functor>
        void registerNative(const std::string& luaFunctionName, Functor fn);

     private:
        LuaWrapper luaState;

        size_t pushVariableValue(const std::string& variableName);
        void pushGlobalOrField(const std::string& value, size_t level);
        std::vector<std::string> getVariableComponents(const std::string& variableName) const;
    };

    inline Lua::Lua(const std::string& filename) : luaState(filename) { }

    template<typename T>
    inline void Lua::set(const std::string& variableName, const T& value) {
        std::vector<std::string> components = getVariableComponents(variableName);

        for (size_t i = 0; i < components.size() - 1; ++i) {
            pushGlobalOrField(components[i], i);
        }

        luaState.pushValue(value);
        luaState.setField(components[components.size() - 1]);
        luaState.pop(components.size() - 1);
    }

    inline void Lua::eval(const std::string& code) {
        luaState.eval(code);
    }

    template<typename T>
    inline T Lua::get(const std::string& variableName) {
        size_t level = pushVariableValue(variableName);
        T result = luaState.get<T>();
        luaState.pop(level + 1);
        return result;
    }

    template<typename T, typename... Args>
    inline T Lua::call(const std::string& functionName, Args&&... args) {
        luaState.pushGlobal(functionName);
        (luaState.pushValue(std::forward<Args>(args)), ...);

        if constexpr (!std::is_same_v<T, void>) {
            luaState.call(sizeof...(args), 1);
            T result = luaState.get<T>();
            luaState.pop();
            return result;
        } else {
            luaState.call(sizeof...(args), 0);
        }
    }

    template <typename Functor>
    inline void Lua::registerNative(const std::string& luaFunctionName, Functor fn) {
        luaState.pushFunction(fn);
        luaState.setGlobal(luaFunctionName);
    }

    inline size_t Lua::pushVariableValue(const std::string& variableName) {
        std::vector<std::string> components = getVariableComponents(variableName);

        for (size_t i = 0; i < components.size(); ++i) {
            pushGlobalOrField(components[i], i);
        }

        return components.size();
    }

    inline void Lua::pushGlobalOrField(const std::string& value, size_t level) {
        if (level == 0) {
            luaState.pushGlobal(value);
        } else {
            luaState.pushField(value);
        }

        assert(!luaState.isNil());
    }

    inline std::vector<std::string> Lua::getVariableComponents(
        const std::string& variableName
    ) const {
        std::vector<std::string> result;
        std::string var;

        for (char ch : variableName) {
            if (ch != '.') {
                var += ch;
                continue;
            }

            result.push_back(var);
            var = "";
        }

        result.push_back(var);
        return result;
    }

}

#endif
