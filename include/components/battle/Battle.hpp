#ifndef BATTLE_HPP
#define BATTLE_HPP

#include <deque>
#include <unordered_set>
#include <vector>
#include "../../battle/Move.hpp"
#include "../../engine/entity-system/types.hpp"

struct BoundFlag {
    engine::entitysystem::Entity target;
    std::string flag;
};

struct UsedMove {
    engine::entitysystem::Entity user;
    engine::entitysystem::Entity target;
    Move* move;
    int moveIndex;
};

struct Battle {
    engine::entitysystem::Entity playerPokemon;
    engine::entitysystem::Entity opponentPokemon;
    std::unordered_set<std::string> playerTeamFlags;
    std::unordered_set<std::string> opponentTeamFlags;
    std::deque<UsedMove> usedMoves;
    std::vector<std::pair<UsedMove, int>> activeMoves;
};

#endif
