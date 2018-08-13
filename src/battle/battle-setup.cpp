#include "battle/battle-setup.hpp"

#include <random>
#include "battle/EncounterData.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

int random(int min, int max) {
    static std::random_device randomDevice;
    static std::mt19937 generator(randomDevice());
    std::uniform_int_distribution distribution(min, max);
    return distribution(generator);
}

std::string findWildBattleOpponent(
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

    EncounterData& chosenEncounter = possibleEncounters.at(chosenEncounterIndex);
    std::string chosenPokemon = chosenEncounter.pokemon;
    int chosenLevel = random(chosenEncounter.minLevel, chosenEncounter.maxLevel);

    // TODO: continue this
    return "";
}

void setupWildEncounter(
    const std::string& mapId,
    engine::entitysystem::Entity battle,
    CoreStructures& gameData
) {
    // TODO
}
