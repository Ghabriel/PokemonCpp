#ifndef OVERWORLD_UTILS_HPP
#define OVERWORLD_UTILS_HPP

#include <utility>
#include "../CoreStructures.hpp"
#include "../engine/entity-system/types.hpp"
#include "../engine/scripting-system/forward-declarations.hpp"

std::pair<int, int> getTargetTile(
    engine::entitysystem::Entity player,
    CoreStructures& gameData
);

engine::scriptingsystem::Lua& getMapScripts(
    engine::entitysystem::Entity map,
    CoreStructures& gameData
);

#endif
