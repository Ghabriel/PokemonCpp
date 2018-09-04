#include "states/BattleState.hpp"

#include "battle/data/Pokemon.hpp"
#include "battle/text-providers/EventTextProvider.hpp"
#include "components/battle/Battle.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "engine/entity-system/include.hpp"

BattleState::BattleState(CoreStructures& gameData)
 : gameData(gameData),
   battleEntity(createEntity(gameData)),
   textProvider(new EventTextProvider()) { }

void BattleState::onEnterImpl() {
    battleController = BattleController(battleEntity, *textProvider, gameData);
    interactiveLayer = InteractiveLayer(battleEntity, battleController, gameData);
    battleSetup = BattleSetup(battleEntity, battleController, interactiveLayer, gameData);
    battleSetup.startBattle();
}

void BattleState::onExitImpl() {
    interactiveLayer.abort();
    music("bgm-wild-battle", gameData).stop();
    deleteEntity(battleEntity, gameData);
    gameData.componentManager->cleanup<Battle, Pokemon>();
}

void BattleState::executeImpl() {
    interactiveLayer.tick();
}
