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
    bool pressingDirectionKey = false;

    void registerInputContext();
    void onEnterImpl() override;
    void onExitImpl() override;
    void executeImpl() override;

    void processMovingEntities();
    void adjustPlayerSpritePosition();

    void onPressDirectionKey(Direction);
    void startWalking();
    void stopWalking();
    void onChangePlayerDirection();
    void updatePlayerAnimation();
    bool isPlayerNearlyAlignedToTile() const;
    void alignPlayerToNearestTile();
};

#endif
