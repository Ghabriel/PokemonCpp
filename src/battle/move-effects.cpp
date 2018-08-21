#include "battle/move-effects.hpp"

#include <queue>
#include "battle/battle-utils.hpp"
#include "battle/Move.hpp"
#include "battle/Pokemon.hpp"
#include "battle/PokemonSpeciesData.hpp"
#include "battle/random.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "events/ImmediateEvent.hpp"
#include "EventQueue.hpp"
#include "lua-native-functions.hpp"

namespace {
    CoreStructures* gameData;
    Pokemon* user;
    Pokemon* target;
    Move* move;

    template<typename TEvent, typename... Args>
    void enqueueEvent(Args&&... args) {
        EventQueue& queue = resource<EventQueue>("move-event-queue", *gameData);
        queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
    }
}

void effects::internal::setGameData(CoreStructures& _gameData) {
    gameData = &_gameData;
}

void effects::internal::setMoveUser(Pokemon& pokemon) {
    user = &pokemon;
}

void effects::internal::setMoveTarget(Pokemon& pokemon) {
    target = &pokemon;
}

void effects::internal::setMove(Move& _move) {
    move = &_move;
}

void effects::damage() {
    PokemonSpeciesData& targetSpecies = getSpecies(*target, *gameData);
    float type = getTypeEffectiveness(targetSpecies, *move);

    if (type < 0.1) {
        lua::showBattleText("It doesn't affect " + target->displayName + "...");
        return;
    }

    PokemonSpeciesData& userSpecies = getSpecies(*user, *gameData);
    int attack = getAttackStatForMove(*user, *move);
    int defense = getDefenseStatForMove(*target, *move);
    int baseDamage = (((2 * user->level) / 5 + 2) * move->power * (attack / defense)) / 50 + 2;
    float targets = 1; // TODO: handle multi-target moves
    float weather = 1; // TODO
    float critical = 1; // TODO
    float rand = random(217, 255) / 255.0;
    float stab = (move->type == userSpecies.types[0] || move->type == userSpecies.types[1])
        ? 1.5
        : 1; // TODO: handle Adaptability
    float burn = 1; // TODO
    float others = 1; // TODO
    float modifier = targets * weather * critical * rand * stab * type * burn * others;
    int damage = baseDamage * modifier;

    if (damage == 0) {
        damage = 1;
    }

    enqueueEvent<ImmediateEvent>([damage] {
        target->currentHP -= damage;
    });
}

void lowerStat(int stat, int levels) {
    // TODO
}

void raiseStat(int stat, int levels) {
    // TODO
}
