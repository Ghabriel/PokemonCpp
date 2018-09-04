#ifndef BATTLE_STATE_HPP
#define BATTLE_STATE_HPP

#include <memory>
#include "../battle/BattleController.hpp"
#include "../battle/BattleSetup.hpp"
#include "../battle/TextProvider.hpp"
#include "../battle/InteractiveLayer.hpp"
#include "../engine/entity-system/types.hpp"
#include "../engine/state-system/State.hpp"

struct CoreStructures;

class BattleState : public engine::statesystem::State {
    using Entity = engine::entitysystem::Entity;
 public:
    explicit BattleState(CoreStructures& gameData);

 private:
    CoreStructures& gameData;
    Entity battleEntity;
    BattleController battleController;
    InteractiveLayer interactiveLayer;
    BattleSetup battleSetup;
    std::unique_ptr<TextProvider> textProvider;

    void onEnterImpl() override;
    void onExitImpl() override;
    void executeImpl() override;
};

#endif
