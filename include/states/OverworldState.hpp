#ifndef OVERWORLD_STATE_HPP
#define OVERWORLD_STATE_HPP

#include "../components/Direction.hpp"
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
    Entity map;

    void executeImpl() override;
    void onEnterImpl() override;
    void onExitImpl() override;
    void registerInputContext();

    void onPressDirectionKey(Direction);
    void walk();
    void onChangePlayerDirection();
};

#endif
