#include "battle/helpers/battle-utils.hpp"

#include <cmath>
#include "battle/data/Move.hpp"
#include "battle/data/Pokemon.hpp"
#include "battle/data/PokemonSpeciesData.hpp"
#include "battle/helpers/random.hpp"
#include "components/battle/Battle.hpp"
#include "components/battle/VolatileData.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

using engine::entitysystem::Entity;

namespace {
    constexpr float getStatStageMultiplier(int stage) {
        int absStage = std::abs(stage);
        return stage >= 0 ? (2 + absStage) / 2.0 : 2.0 / (2 + absStage);
    }

    constexpr float getAccuracyStatStageMultiplier(int stage) {
        int absStage = std::abs(stage);
        return stage >= 0 ? (3 + absStage) / 3.0 : 3.0 / (3 + absStage);
    }
}

int getAttackStatForMove(
    Entity pokemon,
    const Move& move,
    CoreStructures& gameData,
    StatFlags flags
) {
    if (move.kind == "Physical") {
        return getEffectiveStat(pokemon, Stat::Attack, gameData, flags);
    }

    if (move.kind == "Special") {
        return getEffectiveStat(pokemon, Stat::SpecialAttack, gameData, flags);
    }

    return 0;
}

int getDefenseStatForMove(
    Entity pokemon,
    const Move& move,
    CoreStructures& gameData,
    StatFlags flags
) {
    if (move.kind == "Physical") {
        return getEffectiveStat(pokemon, Stat::Defense, gameData, flags);
    }

    if (move.kind == "Special") {
        return getEffectiveStat(pokemon, Stat::SpecialDefense, gameData, flags);
    }

    return 0;
}

int getEffectiveStat(
    Entity pokemon,
    Stat stat,
    CoreStructures& gameData,
    StatFlags flags
) {
    int statId = static_cast<int>(stat);
    int baseStatValue = data<Pokemon>(pokemon, gameData).stats[statId];
    int currentStage = data<VolatileData>(pokemon, gameData).statStages[statId];

    switch (flags) {
        case StatFlags::All:
            break;
        case StatFlags::IgnorePositive:
            currentStage = std::min(0, currentStage);
            break;
        case StatFlags::IgnoreNegative:
            currentStage = std::max(0, currentStage);
            break;
    }

    return baseStatValue * getStatStageMultiplier(currentStage);
}

bool hasUsableMoves(
    engine::entitysystem::Entity pokemon,
    CoreStructures& gameData
) {
    const auto& ppList = data<Pokemon>(pokemon, gameData).pp;

    for (int pp : ppList) {
        if (pp > 0) {
            return true;
        }
    }

    return false;
}

bool checkMiss(
    engine::entitysystem::Entity user,
    engine::entitysystem::Entity target,
    const Move& move,
    CoreStructures& gameData
) {
    if (move.accuracy == 0) {
        return false;
    }

    VolatileData& userData = data<VolatileData>(user, gameData);
    VolatileData& targetData = data<VolatileData>(target, gameData);
    int accuracyStage = std::clamp(userData.statStages[6] - targetData.statStages[7], -6, 6);
    float accuracyStageMultiplier = getAccuracyStatStageMultiplier(accuracyStage);
    float hitRate = move.accuracy * accuracyStageMultiplier;
    return random(1, 100) > hitRate;
}

bool checkCritical(
    engine::entitysystem::Entity user,
    engine::entitysystem::Entity target,
    const Move& move,
    CoreStructures& gameData
) {
    int criticalHitStage = data<VolatileData>(user, gameData).criticalHitStage;
    int chancesIn24;

    switch (criticalHitStage) {
        case 0:
            chancesIn24 = 1;
            break;
        case 1:
            chancesIn24 = 3;
            break;
        case 2:
            chancesIn24 = 12;
            break;
        default:
            chancesIn24 = 24;
            break;
    }

    return random(1, 24) <= chancesIn24;
}

PokemonSpeciesData& getSpecies(const Pokemon& pokemon, CoreStructures& gameData) {
    return resource<PokemonSpeciesData>("pokemon-" + pokemon.species, gameData);
}

float getTypeEffectiveness(const PokemonSpeciesData& species, const Move& move) {
    static const std::array<std::array<float, 18>, 18> typeTable = {{
        {1, 1, 1, 1, 1, 0.5, 1, 0, 0.5, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {2, 1, 0.5, 0.5, 1, 2, 0.5, 0, 2, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5},
        {1, 2, 1, 1, 1, 0.5, 2, 1, 0.5, 1, 1, 2, 0.5, 1, 1, 1, 1, 1},
        {1, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 0, 1, 1, 2, 1, 1, 1, 1, 1, 2},
        {1, 1, 0, 2, 1, 2, 0.5, 1, 2, 2, 1, 0.5, 2, 1, 1, 1, 1, 1},
        {1, 0.5, 2, 1, 0.5, 1, 2, 1, 0.5, 2, 1, 1, 1, 1, 2, 1, 1, 1},
        {1, 0.5, 0.5, 0.5, 1, 1, 1, 0.5, 0.5, 0.5, 1, 2, 1, 2, 1, 1, 2, 0.5},
        {0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 1},
        {1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 1, 2, 1, 1, 2},
        {1, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5, 0.5, 2, 1, 1, 2, 0.5, 1, 1},
        {1, 1, 1, 1, 2, 2, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 1, 0.5, 1, 1},
        {1, 1, 0.5, 0.5, 2, 2, 0.5, 1, 0.5, 0.5, 2, 0.5, 1, 1, 1, 0.5, 1, 1},
        {1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 0.5, 1, 1},
        {1, 2, 1, 2, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 0.5, 1, 1, 0, 1},
        {1, 1, 2, 1, 2, 1, 1, 1, 0.5, 0.5, 0.5, 2, 1, 1, 0.5, 2, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 1, 1, 2, 1, 0},
        {1, 0.5, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5},
        {1, 2, 1, 0.5, 1, 1, 1, 1, 0.5, 0.5, 1, 1, 1, 1, 1, 2, 2, 1},
    }};

    static const std::unordered_map<std::string, size_t> typeMapping = {
        {"Normal", 0},
        {"Fighting", 1},
        {"Flying", 2},
        {"Poison", 3},
        {"Ground", 4},
        {"Rock", 5},
        {"Bug", 6},
        {"Ghost", 7},
        {"Steel", 8},
        {"Fire", 9},
        {"Water", 10},
        {"Grass", 11},
        {"Electric", 12},
        {"Psychic", 13},
        {"Ice", 14},
        {"Dragon", 15},
        {"Dark", 16},
        {"Fairy", 17},
    };

    if (move.type == "???") {
        return 1;
    }

    size_t moveType = typeMapping.at(move.type);
    float result = typeTable[moveType][typeMapping.at(species.types[0])];

    if (species.types.size() > 1) {
        result *= typeTable[moveType][typeMapping.at(species.types[1])];
    }

    return result;
}

int calculateExpGain(
    const Pokemon& winner,
    const Pokemon& fainted,
    const PokemonSpeciesData& faintedSpecies
) {
    float luckyEgg = 1; // TODO: handle Lucky Egg
    int shareFactor = 1; // TODO: handle Exp. Share
    float factor1 = (faintedSpecies.baseExp * fainted.level) / (5 * shareFactor);
    float factor2 = std::pow(2 * fainted.level + 10, 2.5);
    float factor3 = std::pow(fainted.level + winner.level + 10, 2.5);
    return (factor1 * (factor2 / factor3) + 1) * luckyEgg;
}

std::vector<Entity> getPokemonList(Battle& battle) {
    std::vector<Entity> result;
    result.reserve(battle.playerTeam.size() + battle.opponentTeam.size());

    for (const auto& entity : battle.playerTeam) {
        result.push_back(entity);
    }

    for (const auto& entity : battle.opponentTeam) {
        result.push_back(entity);
    }

    return result;
}
