#ifndef BATTLE_HPP
#define BATTLE_HPP

#include "../../engine/entity-system/types.hpp"

struct Battle {
    engine::entitysystem::Entity playerPokemon;
    engine::entitysystem::Entity opponentPokemon;
};

#endif
