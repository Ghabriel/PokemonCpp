#ifndef BATTLE_STATE_HPP
#define BATTLE_STATE_HPP

#include "../engine/entity-system/types.hpp"
#include "../engine/state-system/include.hpp"

struct CoreStructures;
struct Battle;

class BattleState : public engine::statesystem::State {
    using Entity = engine::entitysystem::Entity;
 public:
    BattleState(CoreStructures& gameData);

 private:
    CoreStructures& gameData;
    Entity battleEntity;
    Battle* battle;
    size_t selectedAction;

    void registerInputContext();
    void onEnterImpl() override;
    void onExitImpl() override;
    void executeImpl() override;

    void showText(const std::string&);
};

#endif
