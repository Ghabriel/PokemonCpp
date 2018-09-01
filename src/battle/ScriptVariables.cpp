#include "battle/ScriptVariables.hpp"

#include "battle/battle-utils.hpp"
#include "battle/Pokemon.hpp"
#include "components/battle/Battle.hpp"
#include "constants.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

ScriptVariables::ScriptVariables(CoreStructures& gameData) : gameData(gameData) {}

void ScriptVariables::setBattle(Battle& _battle) {
    battle = &_battle;
}

void ScriptVariables::updateScriptVariables() {
    std::vector<Entity> pokemonList {
        battle->playerPokemon,
        battle->opponentPokemon,
    };

    updateScriptVariables(pokemonList, script("ai", gameData));
    updateScriptVariables(pokemonList, script("moves", gameData));
}

void ScriptVariables::updateScriptUserPointer(Entity user) {
    std::string index = std::to_string(getPokemonIndex(user));
    script("ai", gameData).eval("user = pokemon" + index);
}

void ScriptVariables::updateScriptTargetPointer(Entity target) {
    std::string index = std::to_string(getPokemonIndex(target));
    script("ai", gameData).eval("target = pokemon" + index);
}

size_t ScriptVariables::getPokemonIndex(Entity entity) {
    return (entity == battle->playerPokemon) ? 0 : 1;
}

void ScriptVariables::updateScriptVariables(const std::vector<Entity>& pokemonList, Lua& script) {
    for (size_t i = 0; i < pokemonList.size(); ++i) {
        std::string varName = "pokemon" + std::to_string(i);
        Entity pokemonEntity = pokemonList[i];
        const Pokemon& currentPokemon = data<Pokemon>(pokemonEntity, gameData);

        const auto getStat = [&](Stat stat) {
            return getEffectiveStat(pokemonEntity, stat, gameData);
        };

        script.set(varName + ".species", currentPokemon.species);
        script.set(varName + ".nature", static_cast<int>(currentPokemon.nature));
        script.set(varName + ".heldItem", currentPokemon.heldItem);
        script.set(varName + ".ability", currentPokemon.ability);

        for (size_t j = 0; j < constants::MOVE_LIMIT; ++j) {
            if (j < currentPokemon.moves.size()) {
                script.set(varName + ".move" + std::to_string(j), currentPokemon.moves[j]);
                script.set(varName + ".pp" + std::to_string(j), currentPokemon.pp[j]);
            } else {
                script.set(varName + ".move" + std::to_string(j), std::string{});
                script.set(varName + ".pp" + std::to_string(j), std::string{});
            }
        }

        script.set<int>(varName + ".moveCount", currentPokemon.moves.size());

        script.set(varName + ".gender", static_cast<int>(currentPokemon.gender));
        script.set(varName + ".form", currentPokemon.form);
        script.set(varName + ".displayName", currentPokemon.displayName);
        script.set(varName + ".status", static_cast<int>(currentPokemon.status));
        script.set(varName + ".asleepRounds", currentPokemon.asleepRounds);
        script.set(varName + ".level", currentPokemon.level);
        script.set(varName + ".hp", getStat(Stat::HP));
        script.set(varName + ".attack", getStat(Stat::Attack));
        script.set(varName + ".defense", getStat(Stat::Defense));
        script.set(varName + ".specialAttack", getStat(Stat::SpecialAttack));
        script.set(varName + ".specialDefense", getStat(Stat::SpecialDefense));
        script.set(varName + ".speed", getStat(Stat::Speed));
        script.set(varName + ".currentHP", static_cast<int>(currentPokemon.currentHP));
    }
}
