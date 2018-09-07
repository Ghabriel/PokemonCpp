#ifndef BOUND_MOVE_HPP
#define BOUND_MOVE_HPP

#include "../../engine/entity-system/types.hpp"

struct Move;

struct BoundMove {
    engine::entitysystem::Entity user;
    engine::entitysystem::Entity target;
    Move* move;
    int moveIndex; // for PP deduction
};

#endif
