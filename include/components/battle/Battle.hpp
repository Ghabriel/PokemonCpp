#ifndef BATTLE_HPP
#define BATTLE_HPP

#include <unordered_map>
#include <vector>
#include "../../battle/data/BoundMove.hpp"
#include "../../battle/data/Flag.hpp"
// #include "../../battle/data/Move.hpp"
#include "../../engine/entity-system/types.hpp"

struct Battle {
    using Entity = engine::entitysystem::Entity;
    std::vector<Entity> playerTeam;
    std::vector<Entity> opponentTeam;
    std::unordered_map<size_t, std::vector<Flag>> playerTeamPositionFlags;
    std::unordered_map<size_t, std::vector<Flag>> opponentTeamPositionFlags;
    std::unordered_map<Entity, std::vector<Flag>> pokemonFlags;
    std::vector<BoundMove> usedMoves;
    std::vector<BoundMove> moveHistory;
};

#endif
