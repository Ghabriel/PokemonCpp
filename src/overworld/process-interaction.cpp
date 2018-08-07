#include "overworld/process-interaction.hpp"

#include <utility>
#include "engine/scripting-system/include.hpp"
#include "overworld/overworld-utils.hpp"

using engine::entitysystem::Entity;

void processInteraction(
    CoreStructures& gameData,
    Entity player,
    Entity map
) {
    std::pair<int, int> targetTile = getTargetTile(player, gameData);
    auto& mapScripts = getMapScripts(map, gameData);
    mapScripts.call<void>("interact", targetTile.first, targetTile.second);
}
