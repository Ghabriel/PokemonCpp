#include "overworld/on-tile-step.hpp"

#include <cmath>
#include <utility>
#include "components/Position.hpp"
#include "core-functions.hpp"
#include "engine/scripting-system/include.hpp"
#include "overworld/overworld-utils.hpp"

using engine::entitysystem::Entity;

bool onTileStep(
    CoreStructures& gameData,
    Entity player,
    Entity map
) {
    Position& playerPosition = data<Position>(player, gameData);
    auto& mapScripts = getMapScripts(map, gameData);
    return mapScripts.call<bool>(
        "onTileStep",
        std::round(playerPosition.x),
        std::round(playerPosition.y)
    );
}
