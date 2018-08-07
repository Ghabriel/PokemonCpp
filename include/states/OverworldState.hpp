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

    bool isMovingTowardsBlockedTile() const;
    bool isMoving() const;
    void processMovingEntities();
    void adjustPlayerSpritePosition();
    void adjustCameraPosition();

    void onPressDirectionKey(Direction);
    void startWalking();
    void stopWalking();
    bool isPlayerNearlyAlignedToTile() const;
    void alignPlayerToNearestTile();
};

#endif
