#ifndef PLAYER_MOVE_EVENT_HPP
#define PLAYER_MOVE_EVENT_HPP

#include "../../components/Direction.hpp"
#include "../../components/Position.hpp"
#include "../../engine/entity-system/types.hpp"
#include "../../events/Event.hpp"

struct CoreStructures;

class PlayerMoveEvent : public Event {
    using Entity = engine::entitysystem::Entity;
 public:
    PlayerMoveEvent(
        Direction,
        int numTiles,
        Entity player,
        CoreStructures& gameData
    );

 private:
    Direction direction;
    int numTiles;
    Entity player;
    CoreStructures& gameData;
    Position initialPosition;

    void onStartImpl() override;
    bool tickImpl() override;
};


#endif
