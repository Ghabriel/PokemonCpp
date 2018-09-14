#include "battle/prepare-battle-scripts.hpp"

#include "components/battle/Battle.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

void prepareScriptFile(engine::scriptingsystem::Lua& script) {
    script.eval(
        "pokemonMetatable = {}\n"
        "function pokemonMetatable:__index(key)\n"
            "return getPokemonProperty(self.id, key)\n"
        "end\n"

        "for _, pokemon in ipairs(userTeam) do\n"
            "setmetatable(pokemon, pokemonMetatable)\n"
        "end\n"

        "for _, pokemon in ipairs(targetTeam) do\n"
            "setmetatable(pokemon, pokemonMetatable)\n"
        "end\n"

        "setmetatable(user, pokemonMetatable)\n"
        "setmetatable(target, pokemonMetatable)\n"
        // TODO: move metatable
    );
}

void prepareBattleScripts(CoreStructures& gameData) {
    prepareScriptFile(script("moves", gameData));
}
