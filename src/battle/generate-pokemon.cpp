#include "battle/generate-pokemon.hpp"

#include "battle/Pokemon.hpp"
#include "battle/random.hpp"

Nature pickNature() {
    return static_cast<Nature>(random(0, static_cast<int>(Nature::Count) - 1));
}

Pokemon generatePokemon(const std::string& species, int level) {
    // TODO
    return {};
}
