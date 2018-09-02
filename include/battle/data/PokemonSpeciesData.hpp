#ifndef POKEMON_SPECIES_DATA_HPP
#define POKEMON_SPECIES_DATA_HPP

#include <array>
#include <string>
#include <utility>
#include <vector>
#include "../../engine/resource-system/json/include.hpp"

struct EvolutionData {
    std::string pokemon;
    JsonValue method;
};

struct PokemonSpeciesData {
    std::string displayName;
    int nationalNumber;
    std::vector<std::string> types;
    std::array<int, 6> baseStats;
    std::string maleRatio;
    std::string growthRate;
    int baseExp;
    std::array<int, 6> effortPoints;
    int captureRate;
    int baseHappiness;
    std::vector<std::string> abilities;
    std::vector<std::string> hiddenAbilities;
    std::vector<std::pair<int, std::string>> moves;
    std::vector<std::string> eggMoves;
    std::vector<std::string> eggGroups;
    int eggSteps;
    float height;
    float weight;
    std::string color;
    int shape;
    std::string habitat;
    std::string kind;
    std::string pokedexDescription;
    int battlePlayerY;
    int battleEnemyY;
    int battleAltitude;
    std::vector<EvolutionData> evolutions;
};

#endif
