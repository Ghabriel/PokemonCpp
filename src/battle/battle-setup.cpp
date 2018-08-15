#include "battle/battle-setup.hpp"

#include "battle/EncounterData.hpp"
#include "battle/generate-pokemon.hpp"
#include "battle/Pokemon.hpp"
#include "battle/random.hpp"
#include "components/Battle.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

Pokemon findWildBattleOpponent(
    const std::string& mapId,
    engine::entitysystem::Entity battle,
    CoreStructures& gameData
) {
    MapEncounterData& mapEncounters = resource<MapEncounterData>(
        "encounters-" + mapId,
        gameData
    );

    // TODO: get the correct environment
    const std::string environment = "land";

    const auto& encounterData = mapEncounters.encounterData;
    const std::vector<EncounterData>& possibleEncounters = encounterData.at(environment);

    // TODO: randomize the chosen index
    size_t chosenEncounterIndex = 0;

    const EncounterData& chosenEncounter = possibleEncounters.at(chosenEncounterIndex);
    std::string chosenSpecies = chosenEncounter.pokemon;
    int chosenLevel = random(chosenEncounter.minLevel, chosenEncounter.maxLevel);

    return generatePokemon(*gameData.resourceStorage, chosenSpecies, chosenLevel);
}

void setupWildEncounter(
    const std::string& mapId,
    engine::entitysystem::Entity battle,
    CoreStructures& gameData
) {
    Pokemon player = generatePokemon(*gameData.resourceStorage, "Rattata", 3);
    Pokemon opponent = findWildBattleOpponent(mapId, battle, gameData);
    addComponent(battle, Battle{player, opponent}, gameData);
}