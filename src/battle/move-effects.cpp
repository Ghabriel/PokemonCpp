#include "battle/move-effects.hpp"

#include <queue>
#include "battle/battle-utils.hpp"
#include "battle/Move.hpp"
#include "battle/Pokemon.hpp"
#include "battle/PokemonSpeciesData.hpp"
#include "battle/random.hpp"
#include "constants.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "engine/scripting-system/include.hpp"
#include "events/ImmediateEvent.hpp"
#include "events/TextEvent.hpp"
#include "EventQueue.hpp"
#include "lua-native-functions.hpp"

namespace {
    CoreStructures* gameData;
    engine::entitysystem::Entity battle;
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

void effects::internal::setBattle(engine::entitysystem::Entity _battle) {
    battle = _battle;
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

void effects::lowerStat(int stat, int levels) {
    // TODO
    std::string text = target->displayName + "'s " + constants::DISPLAYNAME_STATS[stat] + " ";

    switch (levels) {
        case 1:
            text += "fell!";
            break;
        case 2:
            text += "harshly fell!";
            break;
        default:
            text += "severely fell!";
            break;
    }

    showText(text);
}

void effects::raiseStat(int stat, int levels) {
    // TODO
    std::string text = target->displayName + "'s " + constants::DISPLAYNAME_STATS[stat] + " ";

    switch (levels) {
        case 1:
            text += "rose!";
            break;
        case 2:
            text += "rose sharply!";
            break;
        default:
            text += "rose drastically!";
            break;
    }

    showText(text);
}


void effects::showText(const std::string& content) {
    enqueueEvent<TextEvent>(content, battle, *gameData);
}

void injectNativeBattleFunctions(engine::scriptingsystem::Lua& script) {
    script.registerNative("damage", effects::damage);
    script.registerNative("lowerStat", effects::lowerStat);
    script.registerNative("raiseStat", effects::raiseStat);
    script.registerNative("showText", effects::showText);
}
