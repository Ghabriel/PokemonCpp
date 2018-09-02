#include "battle/BattleSetup.hpp"

#include "battle/helpers/battle-setup.hpp"
#include "battle/InteractiveLayer.hpp"
#include "core-functions.hpp"

BattleSetup::BattleSetup(
    Entity battleEntity,
    BattleController& battleController,
    InteractiveLayer& interactiveLayer,
    CoreStructures& gameData
) : battleEntity(battleEntity),
    battleController(&battleController),
    interactiveLayer(&interactiveLayer),
    gameData(&gameData) { }

void BattleSetup::startBattle() {
    restoreEntity(battleEntity, *gameData);
    setupWildEncounter("map-basic", battleEntity, *gameData);
    interactiveLayer->startBattle();
}

