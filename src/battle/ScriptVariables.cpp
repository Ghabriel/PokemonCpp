#include "battle/ScriptVariables.hpp"

#include <sstream>
#include "battle/data/Pokemon.hpp"
#include "battle/helpers/battle-utils.hpp"
#include "components/battle/Battle.hpp"
#include "constants.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

namespace {
    template<typename T>
    void set(engine::scriptingsystem::Lua& script, const std::string& varName, const T& value) {
        std::stringstream ss;
        ss << varName;
        ss << " = ";
        ss << value;
        script.eval(ss.str());
    }
}

ScriptVariables::ScriptVariables(CoreStructures& gameData) : gameData(&gameData) { }

void ScriptVariables::setBattle(Battle& _battle) {
    battle = &_battle;
}

void ScriptVariables::updateScriptVariables() {
    updateScriptVariables(script("ai", *gameData));
    updateScriptVariables(script("moves", *gameData));
}

void ScriptVariables::updateScriptUserPointer(Entity user) {
    script("ai", *gameData).eval("user = " + findPokemonVariable(user));
}

void ScriptVariables::updateScriptTargetPointer(Entity target) {
    script("ai", *gameData).eval("target = " + findPokemonVariable(target));
}

std::string ScriptVariables::findPokemonVariable(Entity entity) {
    for (size_t i = 0; i < battle->playerTeam.size(); ++i) {
        if (entity == battle->playerTeam[i]) {
            return "playerTeam[" + std::to_string(i) + "]";
        }
    }

    for (size_t i = 0; i < battle->opponentTeam.size(); ++i) {
        if (entity == battle->opponentTeam[i]) {
            return "opponentTeam[" + std::to_string(i) + "]";
        }
    }

    assert(false);
}

void ScriptVariables::updateScriptVariables(Lua& script) {
    for (size_t i = 0; i < battle->playerTeam.size(); ++i) {
        std::string varName = "playerTeam[" + std::to_string(i) + "]";
        updatePokemonVariables(battle->playerTeam[i], varName, script);
    }

    for (size_t i = 0; i < battle->opponentTeam.size(); ++i) {
        std::string varName = "opponentTeam[" + std::to_string(i) + "]";
        updatePokemonVariables(battle->opponentTeam[i], varName, script);
    }
}

void ScriptVariables::updatePokemonVariables(
    Entity pokemonEntity,
    const std::string& varName,
    Lua& script
) {
    const Pokemon& currentPokemon = data<Pokemon>(pokemonEntity, *gameData);

    const auto getStat = [&](Stat stat) {
        return getEffectiveStat(pokemonEntity, stat, *gameData);
    };

    set(script, varName + ".species", currentPokemon.species);
    set(script, varName + ".nature", static_cast<int>(currentPokemon.nature));
    set(script, varName + ".heldItem", currentPokemon.heldItem);
    set(script, varName + ".ability", currentPokemon.ability);

    for (size_t i = 0; i < constants::MOVE_LIMIT; ++i) {
        if (i < currentPokemon.moves.size()) {
            set(script, varName + ".move" + std::to_string(i), currentPokemon.moves[i]);
            set(script, varName + ".pp" + std::to_string(i), currentPokemon.pp[i]);
        } else {
            set(script, varName + ".move" + std::to_string(i), std::string{});
            set(script, varName + ".pp" + std::to_string(i), std::string{});
        }
    }

    set<int>(script, varName + ".moveCount", currentPokemon.moves.size());

    set(script, varName + ".gender", static_cast<int>(currentPokemon.gender));
    set(script, varName + ".form", currentPokemon.form);
    set(script, varName + ".displayName", currentPokemon.displayName);
    set(script, varName + ".status", static_cast<int>(currentPokemon.status));
    set(script, varName + ".asleepRounds", currentPokemon.asleepRounds);
    set(script, varName + ".level", currentPokemon.level);
    set(script, varName + ".hp", getStat(Stat::HP));
    set(script, varName + ".attack", getStat(Stat::Attack));
    set(script, varName + ".defense", getStat(Stat::Defense));
    set(script, varName + ".specialAttack", getStat(Stat::SpecialAttack));
    set(script, varName + ".specialDefense", getStat(Stat::SpecialDefense));
    set(script, varName + ".speed", getStat(Stat::Speed));
    set(script, varName + ".currentHP", static_cast<int>(currentPokemon.currentHP));

    // script.set(varName + ".species", currentPokemon.species);
    // script.set(varName + ".nature", static_cast<int>(currentPokemon.nature));
    // script.set(varName + ".heldItem", currentPokemon.heldItem);
    // script.set(varName + ".ability", currentPokemon.ability);

    // for (size_t j = 0; j < constants::MOVE_LIMIT; ++j) {
    //     if (j < currentPokemon.moves.size()) {
    //         script.set(varName + ".move" + std::to_string(j), currentPokemon.moves[j]);
    //         script.set(varName + ".pp" + std::to_string(j), currentPokemon.pp[j]);
    //     } else {
    //         script.set(varName + ".move" + std::to_string(j), std::string{});
    //         script.set(varName + ".pp" + std::to_string(j), std::string{});
    //     }
    // }

    // script.set<int>(varName + ".moveCount", currentPokemon.moves.size());

    // script.set(varName + ".gender", static_cast<int>(currentPokemon.gender));
    // script.set(varName + ".form", currentPokemon.form);
    // script.set(varName + ".displayName", currentPokemon.displayName);
    // script.set(varName + ".status", static_cast<int>(currentPokemon.status));
    // script.set(varName + ".asleepRounds", currentPokemon.asleepRounds);
    // script.set(varName + ".level", currentPokemon.level);
    // script.set(varName + ".hp", getStat(Stat::HP));
    // script.set(varName + ".attack", getStat(Stat::Attack));
    // script.set(varName + ".defense", getStat(Stat::Defense));
    // script.set(varName + ".specialAttack", getStat(Stat::SpecialAttack));
    // script.set(varName + ".specialDefense", getStat(Stat::SpecialDefense));
    // script.set(varName + ".speed", getStat(Stat::Speed));
    // script.set(varName + ".currentHP", static_cast<int>(currentPokemon.currentHP));
}
