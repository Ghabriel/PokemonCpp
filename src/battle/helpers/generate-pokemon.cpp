#include "battle/helpers/generate-pokemon.hpp"

#include "battle/data/Move.hpp"
#include "battle/data/Pokemon.hpp"
#include "battle/data/PokemonSpeciesData.hpp"
#include "battle/helpers/random.hpp"
#include "constants.hpp"
#include "engine/resource-system/include.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::resourcesystem::ResourceStorage;

Nature generation::pickNature() {
    return static_cast<Nature>(random(0, static_cast<int>(Nature::Count) - 1));
}

std::array<int, 6> generation::pickIVs() {
    return {
        random(0, 31),
        random(0, 31),
        random(0, 31),
        random(0, 31),
        random(0, 31),
        random(0, 31)
    };
}

std::vector<std::string> generation::pickMoves(
    const PokemonSpeciesData& speciesData,
    int level
) {
    std::vector<std::string> result;

    for (auto it = speciesData.moves.rbegin(); it != speciesData.moves.rend(); ++it) {
        const std::pair<int, std::string>& move = *it;

        if (move.first <= level) {
            result.push_back(move.second);
        }

        if (result.size() == constants::MOVE_LIMIT) {
            break;
        }
    }

    return result;
}

std::vector<int> generation::pickPPs(
    ResourceStorage& storage,
    const std::vector<std::string>& moves
) {
    std::vector<int> result;

    for (const auto& moveId : moves) {
        Move& move = storage.get<Move>("move-" + moveId);
        result.push_back(move.pp);
    }

    return result;
}

Gender generation::pickGender(const PokemonSpeciesData& speciesData) {
    if (speciesData.maleRatio == "-") {
        return Gender::Genderless;
    }

    int maleRatio = static_cast<int>(10 * std::stof(speciesData.maleRatio));
    return random(1, 1000) <= maleRatio ? Gender::Male : Gender::Female;
}

void generation::pickStats(const PokemonSpeciesData& speciesData, Pokemon& pokemon) {
    const auto& baseStats = speciesData.baseStats;
    const auto& ev = pokemon.ev;
    const auto& iv = pokemon.iv;
    int level = pokemon.level;

    for (size_t i = 0; i < 6; ++i) {
        pokemon.stats[i] = ((2 * baseStats[i] + iv[i] + (ev[i] / 4)) * level) / 100 + 5;
    }

    pokemon.stats[0] += level + 5;

    int nature = static_cast<int>(pokemon.nature);
    size_t natureIncreasingStat = nature / 5 + 1;
    size_t natureDecreasingStat = nature % 5 + 1;

    if (natureIncreasingStat != natureDecreasingStat) {
        pokemon.stats[natureIncreasingStat] *= 1.1;
        pokemon.stats[natureDecreasingStat] *= 0.9;
    }
}

Pokemon generatePokemon(
    ResourceStorage& storage,
    const std::string& species,
    int level
) {
    using namespace generation;
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
    pokemon.pp = pickPPs(storage, pokemon.moves); // TODO
    pokemon.ppUps = {0, 0, 0, 0};
    pokemon.eggStepsToHatch = -1;
    pokemon.gender = pickGender(speciesData);
    pokemon.form = 0; // TODO
    pokemon.isNicknamed = false;
    pokemon.displayName = speciesData.displayName;
    pokemon.metAtDate = 0; // TODO
    pokemon.metAtLocation = "map-basic"; // TODO
    pokemon.metAtLevel = level;
    pokemon.pokerus = false; // TODO
    pokemon.pokeball = "";
    pokemon.status = StatusCondition::Normal;
    pokemon.asleepRounds = 0;
    pokemon.level = level;
    pokemon.stats = {0, 0, 0, 0, 0, 0};
    pickStats(speciesData, pokemon);
    pokemon.currentHP = pokemon.stats[0];

    return pokemon;
}
