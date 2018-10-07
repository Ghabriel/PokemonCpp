#include "battle/helpers/battle-utils.hpp"

#include <cmath>
#include "battle/data/EntityId.hpp"
#include "battle/data/Move.hpp"
#include "battle/data/Pokemon.hpp"
#include "battle/data/PokemonSpeciesData.hpp"
#include "battle/helpers/random.hpp"
#include "components/battle/Battle.hpp"
#include "components/battle/VolatileData.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

using engine::entitysystem::Entity;

std::vector<Move*> getMovesOf(Entity pokemonEntity, CoreStructures& gameData) {
    return data<std::vector<Move*>>(pokemonEntity, gameData);
}

PokemonSpeciesData& getSpecies(const Pokemon& pokemon, CoreStructures& gameData) {
    return resource<PokemonSpeciesData>("pokemon-" + pokemon.species, gameData);
}

std::vector<Entity> getPokemonList(Battle& battle) {
    std::vector<Entity> result;
    result.reserve(battle.playerTeam.size() + battle.opponentTeam.size());

    for (const auto& entity : battle.playerTeam) {
        result.push_back(entity);
    }

    for (const auto& entity : battle.opponentTeam) {
        result.push_back(entity);
    }

    return result;
}
