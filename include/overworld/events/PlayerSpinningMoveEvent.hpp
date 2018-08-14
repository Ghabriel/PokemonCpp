#ifndef PLAYER_SPINNING_MOVE_EVENT_HPP
#define PLAYER_SPINNING_MOVE_EVENT_HPP

#include "../../components/Direction.hpp"
#include "../../components/Position.hpp"
#include "../../engine/entity-system/types.hpp"
#include "../../events/Event.hpp"

struct CoreStructures;

class PlayerSpinningMoveEvent : public Event {
    using Entity = engine::entitysystem::Entity;
 public:
    PlayerSpinningMoveEvent(
        Direction,
        int numTiles,
        int spinDelayMs,
        bool clockwise,
        Entity player,
        CoreStructures& gameData
    );

 private:
    Direction direction;
    int numTiles;
    int spinDelayMs;
    bool clockwise;
    Entity player;
    CoreStructures& gameData;
    Position initialPosition;
    float timeAccumulator;

    void onStartImpl() override;
    bool tickImpl() override;
};


#endif
