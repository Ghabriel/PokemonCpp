#include "battle/prepare-battle-scripts.hpp"

#include "components/battle/Battle.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

void prepareScriptFile(engine::scriptingsystem::Lua& script) {
    script.eval(
        "pokemonMetatable = {}\n"
        "moveMetatable = {}\n"
        "function pokemonMetatable:__index(key)\n"
            "return getPokemonProperty(self.id, key)\n"
        "end\n"

        "function moveMetatable:__index(key)\n"
            "return getMoveProperty(self.id, key)\n"
        "end\n"

        "for _, pokemon in ipairs(userTeam) do\n"
            "setmetatable(pokemon, pokemonMetatable)\n"
        "end\n"

        "for _, pokemon in ipairs(targetTeam) do\n"
            "setmetatable(pokemon, pokemonMetatable)\n"
        "end\n"

        "setmetatable(user, pokemonMetatable)\n"
        "setmetatable(target, pokemonMetatable)\n"
        "setmetatable(move, moveMetatable)\n"
    );
}

void prepareBattleScripts(CoreStructures& gameData) {
    prepareScriptFile(script("moves", gameData));
}
