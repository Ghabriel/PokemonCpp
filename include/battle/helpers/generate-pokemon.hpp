#ifndef GENERATE_POKEMON_HPP
#define GENERATE_POKEMON_HPP

#include <string>
#include "../../engine/resource-system/forward-declarations.hpp"

struct Pokemon;

Pokemon generatePokemon(
    engine::resourcesystem::ResourceStorage& storage,
    const std::string& species,
    int level
);

#endif
