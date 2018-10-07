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

std::vector<Move*> getMovesOf(
    engine::entitysystem::Entity pokemonEntity,
    CoreStructures& gameData
);

PokemonSpeciesData& getSpecies(const Pokemon& pokemon, CoreStructures& gameData);

std::vector<engine::entitysystem::Entity> getPokemonList(Battle&);

#endif
