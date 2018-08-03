#ifndef OVERWORLD_STATE_HPP
#define OVERWORLD_STATE_HPP

#include "../engine/entity-system/types.hpp"
#include "../engine/state-system/include.hpp"

struct CoreStructures;

class OverworldState : public engine::statesystem::State {
    using Entity = engine::entitysystem::Entity;
 public:
    OverworldState(CoreStructures& gameData);

 private:
    CoreStructures& gameData;
    Entity player;

    void executeImpl() override;
    void onEnterImpl() override;
    void onExitImpl() override;
    void registerInputContext();
};

#endif
