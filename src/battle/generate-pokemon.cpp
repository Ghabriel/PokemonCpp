#include "battle/generate-pokemon.hpp"

#include "battle/Pokemon.hpp"
#include "battle/PokemonSpeciesData.hpp"
#include "battle/random.hpp"
#include "engine/resource-system/include.hpp"

using engine::resourcesystem::ResourceStorage;

Nature pickNature() {
    return static_cast<Nature>(random(0, static_cast<int>(Nature::Count) - 1));
}

std::array<int, 6> pickIVs() {
    return {
        random(0, 31),
        random(0, 31),
        random(0, 31),
        random(0, 31),
        random(0, 31),
        random(0, 31)
    };
}

std::vector<std::string> pickMoves(const PokemonSpeciesData& speciesData, int level) {
    std::vector<std::string> result;

    for (auto it = speciesData.moves.rbegin(); it != speciesData.moves.rend(); ++it) {
        const std::pair<int, std::string>& move = *it;

        if (move.first <= level) {
            result.push_back(move.second);
        }

        if (result.size() == 4) {
            break;
        }
    }

    return result;
}

Gender pickGender(const PokemonSpeciesData& speciesData) {
    int maleRatio = static_cast<int>(10 * speciesData.maleRatio);

    // TODO: handle genderless Pokémon
    return random(1, 1000) <= maleRatio ? Gender::Male : Gender::Female;
}

void pickStats(Pokemon& pokemon) {
    // TODO
}

Pokemon generatePokemon(
    ResourceStorage& storage,
    const std::string& species,
    int level
) {
    PokemonSpeciesData& speciesData = storage.get<PokemonSpeciesData>("pokemon-" + species);
    Pokemon pokemon;

    pokemon.species = species;
    pokemon.nature = pickNature();
    pokemon.heldItem = "";
    pokemon.experiencePoints = 0;
    pokemon.ability = ""; // TODO
    pokemon.ev = {0, 0, 0, 0, 0, 0};
    pokemon.iv = pickIVs();
    pokemon.moves = pickMoves(speciesData, level);
    pokemon.pp = {}; // TODO
    pokemon.ppUps = {0, 0, 0, 0};
    pokemon.eggStepsToHatch = -1;
    pokemon.gender = pickGender(speciesData);
    pokemon.form = 0; // TODO
    pokemon.nickname = "";
    pokemon.metAtDate = 0; // TODO
    pokemon.metAtLocation = "map-basic"; // TODO
    pokemon.metAtLevel = level;
    pokemon.pokerus = false; // TODO
    pokemon.pokeball = "";
    pokemon.status = StatusCondition::Normal;
    pokemon.asleepRounds = 0;
    pokemon.level = level;
    pokemon.stats = {};
    pickStats(pokemon);
    pokemon.currentHP = pokemon.stats[0];

    return pokemon;
}
