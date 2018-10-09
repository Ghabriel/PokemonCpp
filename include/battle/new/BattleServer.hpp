#ifndef BATTLE_SERVER_HPP
#define BATTLE_SERVER_HPP

#include <vector>
#include "PublicPokemonData.hpp"
#include "../../engine/entity-system/types.hpp"

class BattleServer {
    using Entity = engine::entitysystem::Entity;
 public:
    // Used to show the Pokémon sprites and their name/gender/level/HP/status conditions
    virtual std::vector<PublicPokemonData> getPlayerActivePokemon() const = 0;
    virtual std::vector<PublicPokemonData> getOpponentActivePokemon() const = 0;

    // Used to show each trainer's pokéballs
    virtual std::vector<StatusCondition> getPlayerTeamStatusConditions() const = 0;
    virtual std::vector<StatusCondition> getOpponentTeamStatusConditions() const = 0;

    virtual void sendClientAction(ClientBattleAction*);
    virtual TurnData processTurn();
};

#endif
