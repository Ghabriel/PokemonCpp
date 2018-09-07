#ifndef BATTLE_UTILS_HPP
#define BATTLE_UTILS_HPP

#include <vector>
#include "battle/data/Stat.hpp"
#include "engine/entity-system/types.hpp"

struct Battle;
struct CoreStructures;
struct Move;
struct Pokemon;
struct PokemonSpeciesData;

enum class StatFlags {
    All,
    IgnorePositive,
    IgnoreNegative
};

int getAttackStatForMove(
    engine::entitysystem::Entity pokemon,
    const Move& move,
    CoreStructures& gameData,
    StatFlags flags = StatFlags::All
);

int getDefenseStatForMove(
    engine::entitysystem::Entity pokemon,
    const Move& move,
    CoreStructures& gameData,
    StatFlags flags = StatFlags::All
);

int getEffectiveStat(
    engine::entitysystem::Entity pokemon,
    Stat stat,
    CoreStructures &gameData,
    StatFlags flags = StatFlags::All
);

bool hasUsableMoves(
    engine::entitysystem::Entity pokemon,
    CoreStructures& gameData
);

bool checkMiss(
    engine::entitysystem::Entity user,
    engine::entitysystem::Entity target,
    const Move& move,
    CoreStructures& gameData
);

bool checkCritical(
    engine::entitysystem::Entity user,
    engine::entitysystem::Entity target,
    const Move& move,
    CoreStructures& gameData
);

PokemonSpeciesData& getSpecies(const Pokemon& pokemon, CoreStructures& gameData);

float getTypeEffectiveness(const PokemonSpeciesData& species, const Move& move);

int calculateExpGain(
    const Pokemon& winner,
    const Pokemon& fainted,
    const PokemonSpeciesData& faintedSpecies
);

std::vector<engine::entitysystem::Entity> getPokemonList(Battle&);

#endif
