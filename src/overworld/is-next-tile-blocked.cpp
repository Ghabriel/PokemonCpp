#include "overworld/is-next-tile-blocked.hpp"

#include <utility>
#include "engine/scripting-system/include.hpp"
#include "overworld/overworld-utils.hpp"

using engine::entitysystem::Entity;

bool isNextTileBlocked(
    CoreStructures& gameData,
    Entity player,
    Entity map
) {
    std::pair<int, int> targetTile = getTargetTile(player, gameData);
    auto& mapScripts = getMapScripts(map, gameData);
    return mapScripts.call<bool>("isTileBlocked", targetTile.first, targetTile.second);
}
