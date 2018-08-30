#include "battle/move-effects.hpp"

#include <queue>
#include "battle/battle-utils.hpp"
#include "battle/events/ValueAnimationEvent.hpp"
#include "battle/Move.hpp"
#include "battle/Pokemon.hpp"
#include "battle/PokemonSpeciesData.hpp"
#include "battle/random.hpp"
#include "components/battle/Battle.hpp"
#include "components/battle/VolatileData.hpp"
#include "constants.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "engine/scripting-system/include.hpp"
#include "events/ImmediateEvent.hpp"
#include "events/TextEvent.hpp"
#include "EventQueue.hpp"
#include "lua-native-functions.hpp"

using engine::entitysystem::Entity;

namespace {
    CoreStructures* gameData;
    Entity battle;
    Entity user;
    Entity target;
    Move* move;
    const UsedMove* usedMove;

    bool criticalHitFlag = false;
    bool hitFlag = false;
    int damageBuffer = 0;

    template<typename TEvent, typename... Args>
    void enqueueEvent(Args&&... args) {
        EventQueue& queue = resource<EventQueue>("move-event-queue", *gameData);
        queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
    }
}

void effects::internal::setGameData(CoreStructures& _gameData) {
    gameData = &_gameData;
}

void effects::internal::setBattle(Entity _battle) {
    battle = _battle;
}

void effects::internal::setMoveUser(Entity pokemon) {
    user = pokemon;
}

void effects::internal::setMoveTarget(Entity pokemon) {
    target = pokemon;
}

void effects::internal::setMove(Move& _move) {
    move = &_move;
}

void effects::internal::setUsedMove(const UsedMove& _usedMove) {
    usedMove = &_usedMove;
}

void effects::damage() {
    enqueueEvent<ImmediateEvent>([&] {
        hitFlag = false;
        damageBuffer = 0;
    });

    PokemonSpeciesData& targetSpecies = data<PokemonSpeciesData>(target, *gameData);
    float type = getTypeEffectiveness(targetSpecies, *move);

    if (type < 0.1) {
        showText(
            "It doesn't affect " +
            data<Pokemon>(target, *gameData).displayName + "..."
        );
        return;
    }

    PokemonSpeciesData& userSpecies = data<PokemonSpeciesData>(user, *gameData);
    // "Base Damage" parameters
    StatFlags attackStatFlags = criticalHitFlag ? StatFlags::IgnoreNegative : StatFlags::All;
    StatFlags defenseStatFlags = criticalHitFlag ? StatFlags::IgnorePositive : StatFlags::All;
    int userLevel = data<Pokemon>(user, *gameData).level;
    int attack = getAttackStatForMove(user, *move, *gameData, attackStatFlags);
    int defense = getDefenseStatForMove(target, *move, *gameData, defenseStatFlags);

    // Modifiers
    float targets = 1; // TODO: handle multi-target moves
    float weather = 1; // TODO
    float critical = criticalHitFlag ? 1.5 : 1;
    float rand = random(217, 255) / 255.0;
    float stab = (move->type == userSpecies.types[0] || move->type == userSpecies.types[1])
        ? 1.5
        : 1; // TODO: handle Adaptability
    float burn = 1; // TODO
    float others = 1; // TODO

    int baseDamage = (((2 * userLevel) / 5 + 2) * move->power * (attack / defense)) / 50 + 2;
    float modifier = targets * weather * critical * rand * stab * type * burn * others;
    int damage = baseDamage * modifier;

    if (damage == 0) {
        damage = 1;
    }

    float& targetHP = data<Pokemon>(target, *gameData).currentHP;

    enqueueEvent<ValueAnimationEvent>(
        targetHP,
        std::max(0, static_cast<int>(targetHP - damage)),
        *gameData
    );

    enqueueEvent<ImmediateEvent>([damage] {
        criticalHitFlag = false;
        hitFlag = true;
        damageBuffer = damage;
    });

    if (criticalHitFlag) {
        showText("A critical hit!");
    }
}

void effects::damageWithFixedRecoil(int lostHP) {
    damage();

    enqueueEvent<ImmediateEvent>([&, lostHP] {
        if (hitFlag) {
            Pokemon& userPokemon = data<Pokemon>(user, *gameData);
            float& userHP = userPokemon.currentHP;

            enqueueEvent<ValueAnimationEvent>(
                userHP,
                std::max(0, static_cast<int>(userHP - lostHP)),
                *gameData
            );

            showText(userPokemon.displayName + " is hit with recoil!");
        }
    });
}

void effects::damageWithRecoil(float recoilRate) {
    damage();

    enqueueEvent<ImmediateEvent>([&, recoilRate] {
        if (hitFlag) {
            Pokemon& userPokemon = data<Pokemon>(user, *gameData);
            float& userHP = userPokemon.currentHP;

            enqueueEvent<ValueAnimationEvent>(
                userHP,
                std::max(
                    0,
                    static_cast<int>(userHP - static_cast<int>(damageBuffer * recoilRate))
                ),
                *gameData
            );

            showText(userPokemon.displayName + " is hit with recoil!");
        }
    });
}

void effects::fixedDamage(int lostHP) {
    enqueueEvent<ImmediateEvent>([&] {
        hitFlag = false;
        damageBuffer = 0;
    });

    PokemonSpeciesData& targetSpecies = data<PokemonSpeciesData>(target, *gameData);
    float type = getTypeEffectiveness(targetSpecies, *move);

    if (type < 0.1) {
        showText(
            "It doesn't affect " +
            data<Pokemon>(target, *gameData).displayName + "..."
        );
        return;
    }

    float& targetHP = data<Pokemon>(target, *gameData).currentHP;

    enqueueEvent<ValueAnimationEvent>(
        targetHP,
        std::max(0, static_cast<int>(targetHP - lostHP)),
        *gameData
    );

    enqueueEvent<ImmediateEvent>([lostHP] {
        hitFlag = true;
        damageBuffer = lostHP;
    });
}

void effects::lowerStat(int statId, int levels) {
    int& currentStage = data<VolatileData>(target, *gameData).statStages[statId];

    std::string text =
        data<Pokemon>(target, *gameData).displayName +
        "'s " + constants::DISPLAYNAME_STATS[statId] + " ";

    if (currentStage == -6) {
        text += "won't go any lower!";
    } else {
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

        // TODO: currentStage will be an INVALID REFERENCE!
        enqueueEvent<ImmediateEvent>([&, levels] {
            currentStage = std::max(-6, currentStage - levels);
        });
    }

    showText(text);
}

void effects::raiseStat(int statId, int levels) {
    int& currentStage = data<VolatileData>(target, *gameData).statStages[statId];

    std::string text =
        data<Pokemon>(target, *gameData).displayName +
        "'s " + constants::DISPLAYNAME_STATS[statId] + " ";

    if (currentStage == 6) {
        text += "won't go any higher!";
    } else {
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

        // TODO: currentStage will be an INVALID REFERENCE!
        enqueueEvent<ImmediateEvent>([&, levels] {
            currentStage = std::min(6, currentStage + levels);
        });
    }

    showText(text);
}

void effects::ensureCriticalHit() {
    // TODO: this must be done in the Event Queue
    criticalHitFlag = true;
}

void effects::persist(int numTurns) {
    data<Battle>(battle, *gameData).activeMoves.push_back({*usedMove, numTurns});
}


void effects::showText(const std::string& content) {
    enqueueEvent<TextEvent>(content, battle, *gameData);
}

void injectNativeBattleFunctions(engine::scriptingsystem::Lua& script) {
    script.registerNative("damage", effects::damage);
    script.registerNative("damageWithFixedRecoil", effects::damageWithFixedRecoil);
    script.registerNative("damageWithRecoil", effects::damageWithRecoil);
    script.registerNative("fixedDamage", effects::fixedDamage);
    script.registerNative("lowerStat", effects::lowerStat);
    script.registerNative("raiseStat", effects::raiseStat);
    script.registerNative("ensureCriticalHit", effects::ensureCriticalHit);
    script.registerNative("persist", effects::persist);
    script.registerNative("showText", effects::showText);
}
