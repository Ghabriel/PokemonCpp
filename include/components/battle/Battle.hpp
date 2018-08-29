#ifndef BATTLE_HPP
#define BATTLE_HPP

#include <deque>
#include "../../battle/Move.hpp"
#include "../../engine/entity-system/types.hpp"

struct UsedMove {
    engine::entitysystem::Entity user;
    engine::entitysystem::Entity target;
    Move* move;
    int moveIndex;
};

struct Battle {
    engine::entitysystem::Entity playerPokemon;
    engine::entitysystem::Entity opponentPokemon;
    std::deque<UsedMove> usedMoves;
};

#endif
