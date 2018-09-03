#ifndef GENERATE_POKEMON_HPP
#define GENERATE_POKEMON_HPP

#include <array>
#include <string>
#include <vector>
#include "../../engine/resource-system/forward-declarations.hpp"

enum class Gender;
enum class Nature;
struct Pokemon;
struct PokemonSpeciesData;

namespace generation {
    Nature pickNature();
    std::array<int, 6> pickIVs();
    std::vector<std::string> pickMoves(const PokemonSpeciesData&, int level);
    std::vector<int> pickPPs(
        engine::resourcesystem::ResourceStorage&,
        const std::vector<std::string>& moves
    );
    Gender pickGender(const PokemonSpeciesData&);
    void pickStats(const PokemonSpeciesData&, Pokemon&);
}

Pokemon generatePokemon(
    engine::resourcesystem::ResourceStorage& storage,
    const std::string& species,
    int level
);

#endif
