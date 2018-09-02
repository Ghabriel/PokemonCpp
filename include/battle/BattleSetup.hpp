#ifndef BATTLE_SETUP_CLASS_HPP
#define BATTLE_SETUP_CLASS_HPP

#include "../engine/entity-system/types.hpp"

struct BattleController;
struct CoreStructures;
struct InteractiveLayer;

class BattleSetup {
    using Entity = engine::entitysystem::Entity;
 public:
    BattleSetup() = default;
    BattleSetup(
        Entity battleEntity,
        BattleController&,
        InteractiveLayer&,
        CoreStructures&
    );
    void startBattle();

 private:
    Entity battleEntity;
    BattleController* battleController;
    InteractiveLayer* interactiveLayer;
    CoreStructures* gameData;
};

#endif
