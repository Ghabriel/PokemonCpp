#include "battle/battle-utils.hpp"

#include "battle/Move.hpp"
#include "battle/Pokemon.hpp"
#include "battle/PokemonSpeciesData.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

int getAttackStatForMove(const Pokemon& pokemon, const Move& move) {
    if (move.kind == "Physical") {
        return pokemon.stats[1];
    }

    if (move.kind == "Special") {
        return pokemon.stats[3];
    }

    return 0;
}

int getDefenseStatForMove(const Pokemon& pokemon, const Move& move) {
    if (move.kind == "Physical") {
        return pokemon.stats[2];
    }

    if (move.kind == "Special") {
        return pokemon.stats[4];
    }

    return 0;
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

    size_t moveType = typeMapping.at(move.type);
    float result = typeTable[moveType][typeMapping.at(species.types[0])];

    if (species.types.size() > 1) {
        result *= typeTable[moveType][typeMapping.at(species.types[1])];
    }

    return result;
}
