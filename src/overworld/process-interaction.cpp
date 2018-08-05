#include "overworld/process-interaction.hpp"

#include <iostream>
#include <utility>
#include "engine/scripting-system/include.hpp"
#include "overworld/overworld-utils.hpp"

using engine::entitysystem::Entity;

void write(const std::string& str) {
    std::cout << str << std::endl;
}

void processInteraction(
    CoreStructures& gameData,
    Entity player,
    Entity map
) {
    std::pair<int, int> targetTile = getTargetTile(player, gameData);
    auto& mapScripts = getMapScripts(map, gameData);
    mapScripts.registerNative("write", write);
    mapScripts.call<void>("interact", targetTile.first, targetTile.second);
}
