#ifndef BATTLE_SETUP_HPP
#define BATTLE_SETUP_HPP

#include <string>
#include <utility>
#include "../../engine/entity-system/types.hpp"

struct CoreStructures;

void setupWildEncounter(
    const std::string& mapId,
    engine::entitysystem::Entity battle,
    CoreStructures& gameData
);

#endif
