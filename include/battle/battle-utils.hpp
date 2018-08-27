#ifndef BATTLE_UTILS_HPP
#define BATTLE_UTILS_HPP

#include "battle/Stat.hpp"
#include "engine/entity-system/types.hpp"

struct CoreStructures;
struct Move;
struct Pokemon;
struct PokemonSpeciesData;

int getAttackStatForMove(
    engine::entitysystem::Entity pokemon,
    const Move& move,
    CoreStructures& gameData
);

int getDefenseStatForMove(
    engine::entitysystem::Entity pokemon,
    const Move& move,
    CoreStructures& gameData
);

int getEffectiveStat(
    engine::entitysystem::Entity pokemon,
    Stat stat,
    CoreStructures& gameData
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

PokemonSpeciesData& getSpecies(const Pokemon& pokemon, CoreStructures& gameData);

float getTypeEffectiveness(const PokemonSpeciesData& species, const Move& move);

int calculateExpGain(
    const Pokemon& winner,
    const Pokemon& fainted,
    const PokemonSpeciesData& faintedSpecies
);

#endif
