#include "battle/helpers/move-effects.hpp"

#include <queue>
#include "battle/data/BoundMove.hpp"
#include "battle/data/EntityId.hpp"
#include "battle/data/Flag.hpp"
#include "battle/data/Move.hpp"
#include "battle/data/Pokemon.hpp"
#include "battle/data/PokemonSpeciesData.hpp"
#include "battle/EventManager.hpp"
#include "battle/events/ValueAnimationEvent.hpp"
#include "battle/helpers/battle-utils.hpp"
#include "battle/helpers/random.hpp"
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
    EventManager* eventManager;
    Entity user;
    Entity target;
    Move* move;
    const BoundMove* boundMove;

    bool criticalHitFlag = false;
    bool hitFlag = false;
    int damageBuffer = 0;
    float damageMultiplier = 1;
    bool moveIsNegated = false;

    template<typename TEvent, typename... Args>
    void enqueueEvent(Args&&... args) {
        EventQueue& queue = resource<EventQueue>("move-event-queue", *gameData);
        queue.addEvent(std::make_unique<TEvent>(std::forward<Args>(args)...));
    }

    Entity getPokemonEntity(int entityId) {
        switch (static_cast<EntityId>(entityId)) {
            case EntityId::User:
                return user;
            case EntityId::Target:
                return target;
            case EntityId::UserTeam0:
            case EntityId::UserTeam1:
            case EntityId::UserTeam2:
            case EntityId::UserTeam3:
            case EntityId::UserTeam4:
            case EntityId::TargetTeam0:
            case EntityId::TargetTeam1:
            case EntityId::TargetTeam2:
            case EntityId::TargetTeam3:
            case EntityId::TargetTeam4:
                // TODO
                assert(false);
            case EntityId::Move:
                // should be unreachable
                assert(false);
            default:
                assert(entityId >= 0);
                return static_cast<Entity>(entityId);
        }
    }

    void removeFlagFrom(const std::string& flagId, std::vector<Flag>& flagList) {
        for (auto it = flagList.begin(); it != flagList.end(); ++it) {
            if (it->id == flagId) {
                flagList.erase(it);
                break;
            }
        }
    }

    bool isFlagIn(const std::string& flagId, std::vector<Flag>& flagList) {
        for (const auto& flag : flagList) {
            if (flag.id == flagId) {
                return true;
            }
        }

        return false;
    }

    BoundMove getBoundMove(int pokemonId, int moveIndex) {
        if (pokemonId == static_cast<int>(EntityId::Move)) {
            return *boundMove;
        }

        // TODO: handle special move indices (Struggle, Metronome, etc)
        Move* move = getMovesOf(pokemonId, *gameData)[moveIndex];
        return {pokemonId, -1, move, moveIndex};
    }

}

void effects::internal::setGameData(CoreStructures& _gameData) {
    gameData = &_gameData;
}

void effects::internal::setBattle(Entity _battle) {
    battle = _battle;
}

void effects::internal::setEventManager(EventManager& _eventManager) {
    eventManager = &_eventManager;
}

void effects::internal::setMove(const BoundMove& usedMove) {
    user = usedMove.user;
    target = usedMove.target;
    move = usedMove.move;
    boundMove = &usedMove;
}

void effects::internal::setFlag(const Flag& flag) {
    target = flag.target;
}

void effects::cleanup() {
    criticalHitFlag = false;
    hitFlag = false;
    damageBuffer = 0;
    damageMultiplier = 1;
    moveIsNegated = false;
}

bool effects::isMoveNegated() {
    return moveIsNegated;
}

void effects::damage() {
    enqueueEvent<ImmediateEvent>([&] {
        hitFlag = false;
        damageBuffer = 0;
    });

    float type = lua::call<float>("getTypeEffectiveness", target, *boundMove);

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
    int attack = lua::call<int>("getAttackStatForMove", user, *boundMove, attackStatFlags);
    int defense = lua::call<int>("getDefenseStatForMove", target, *boundMove, defenseStatFlags);

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

    Entity targetCopy = target; // might be changed by beforeDamageInflict
    // TODO: apply this for fixedDamage() (OHKO moves might become bugged)
    eventManager->triggerUserEvents(*boundMove, "beforeDamageInflict");
    damage *= damageMultiplier;

    if (damage == 0) {
        damage = 1;
    }

    float& targetHP = data<Pokemon>(targetCopy, *gameData).currentHP;

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

    float type = lua::call<float>("getTypeEffectiveness", target, *boundMove);

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

void effects::multiplyDamage(float factor) {
    damageMultiplier *= factor;
}

void effects::negateMove() {
    moveIsNegated = true;
}

void effects::addStatusCondition(int entityId, int statusConditionId) {
    Entity entity = getPokemonEntity(entityId);
    Pokemon& pokemon = data<Pokemon>(entity, *gameData);
    pokemon.status = static_cast<StatusCondition>(statusConditionId);
}

void effects::removeStatusCondition(int entityId) {
    Entity entity = getPokemonEntity(entityId);
    Pokemon& pokemon = data<Pokemon>(entity, *gameData);
    pokemon.status = StatusCondition::Normal;
    pokemon.asleepRounds = 0;
}

void effects::sleep(int entityId, int duration) {
    Entity entity = getPokemonEntity(entityId);
    Pokemon& pokemon = data<Pokemon>(entity, *gameData);
    pokemon.status = StatusCondition::Sleep;
    pokemon.asleepRounds = duration;
}

void effects::reduceSleepCounter() {
    data<Pokemon>(target, *gameData).asleepRounds--;
}

void effects::addTimedFlagUser(const std::string& flagId, int duration) {
    auto& flagList = data<Battle>(battle, *gameData).pokemonFlags[user];
    flagList.push_back({user, flagId, duration, 0});
}

void effects::addFlagUser(const std::string& flagId) {
    addTimedFlagUser(flagId, -1);
}

void effects::addTimedFlagTarget(const std::string& flagId, int duration) {
    auto& flagList = data<Battle>(battle, *gameData).pokemonFlags[target];
    flagList.push_back({target, flagId, duration, 0});
}

void effects::addFlagTarget(const std::string& flagId) {
    addTimedFlagTarget(flagId, -1);
}

void effects::removeFlagUser(const std::string& flagId) {
    auto& flagList = data<Battle>(battle, *gameData).pokemonFlags[user];
    removeFlagFrom(flagId, flagList);
}

void effects::removeFlagTarget(const std::string& flagId) {
    auto& flagList = data<Battle>(battle, *gameData).pokemonFlags[target];
    removeFlagFrom(flagId, flagList);
}

bool effects::hasFlag(int entityId, const std::string& flagId) {
    Entity entity = getPokemonEntity(entityId);
    auto& flagList = data<Battle>(battle, *gameData).pokemonFlags[entity];
    return isFlagIn(flagId, flagList);
}

int effects::getStatStage(int entityId, int statId) {
    Entity entity = getPokemonEntity(entityId);
    return data<VolatileData>(entity, *gameData).statStages[statId];
}


int effects::random(int min, int max) {
    return ::random(min, max);
}

std::string effects::getPokemonProperty(int entityId, const std::string& property) {
    Entity pokemonEntity = getPokemonEntity(entityId);
    const Pokemon& currentPokemon = data<Pokemon>(pokemonEntity, *gameData);

    for (size_t i = 0; i < constants::MOVE_LIMIT; ++i) {
        if (property == "move" + std::to_string(i)) {
            return (i < currentPokemon.moves.size()) ? currentPokemon.moves[i] : "";
        }

        if (property == "pp" + std::to_string(i)) {
            return (i < currentPokemon.pp.size()) ? std::to_string(currentPokemon.pp[i]) : "";
        }
    }

    const auto getStat = [&](Stat stat) {
        return lua::call<int>("getEffectiveStat", pokemonEntity, stat, StatFlags::All);
    };

    const auto throwError = [] {
        assert(false);
        return "";
    };

    PokemonSpeciesData& speciesData = data<PokemonSpeciesData>(pokemonEntity, *gameData);
    size_t typeCount = speciesData.types.size();

    return
        // Pokémon data
        (property == "species")          ? currentPokemon.species :
        (property == "nature")           ? std::to_string(static_cast<int>(currentPokemon.nature)) :
        (property == "heldItem")         ? currentPokemon.heldItem :
        (property == "experiencePoints") ? std::to_string(currentPokemon.experiencePoints) :
        (property == "ability")          ? currentPokemon.ability :
        (property == "moveCount")        ? std::to_string(currentPokemon.moves.size()) :
        (property == "gender")           ? std::to_string(static_cast<int>(currentPokemon.gender)) :
        (property == "form")             ? std::to_string(currentPokemon.form) :
        (property == "displayName")      ? currentPokemon.displayName :
        (property == "pokerus")          ? std::to_string(currentPokemon.pokerus) :
        (property == "statusCondition")  ? std::to_string(static_cast<int>(currentPokemon.status)) :
        (property == "asleepRounds")     ? std::to_string(currentPokemon.asleepRounds) :
        (property == "level")            ? std::to_string(currentPokemon.level) :
        (property == "hp")               ? std::to_string(getStat(Stat::HP)) :
        (property == "attack")           ? std::to_string(getStat(Stat::Attack)) :
        (property == "defense")          ? std::to_string(getStat(Stat::Defense)) :
        (property == "specialAttack")    ? std::to_string(getStat(Stat::SpecialAttack)) :
        (property == "specialDefense")   ? std::to_string(getStat(Stat::SpecialDefense)) :
        (property == "speed")            ? std::to_string(getStat(Stat::Speed)) :
        (property == "currentHP")        ? std::to_string(static_cast<int>(currentPokemon.currentHP)) :
        // Species data
        (property == "typeCount")        ? std::to_string(typeCount) :
        (property == "type0")            ? speciesData.types[0] :
        (property == "type1")            ? (typeCount == 1 ? "" : speciesData.types[1]) :
        (property == "baseExp")          ? std::to_string(speciesData.baseExp) :
        (property == "captureRate")      ? std::to_string(speciesData.captureRate) :
        (property == "height")           ? std::to_string(speciesData.height) :
        (property == "weight")           ? std::to_string(speciesData.weight) :
        throwError();
}

std::string effects::getMoveProperty(int pokemonId, int moveIndex, const std::string& property) {
    BoundMove requestedMove = getBoundMove(pokemonId, moveIndex);
    const Pokemon& moveUser = data<Pokemon>(requestedMove.user, *gameData);
    int currentPP = moveUser.pp[requestedMove.moveIndex];

    const auto throwError = [] {
        assert(false);
        return "";
    };

    return
        (property == "displayName") ? move->displayName :
        (property == "type") ? move->type :
        (property == "kind") ? move->kind :
        (property == "power") ? std::to_string(move->power) :
        (property == "accuracy") ? std::to_string(move->accuracy) :
        (property == "currentPP") ? std::to_string(currentPP) :
        (property == "targetType") ? move->targetType :
        (property == "priority") ? std::to_string(move->priority) :
        (property == "flags") ? move->flags :
        throwError();
}

void effects::showText(const std::string& content) {
    enqueueEvent<TextEvent>(content, battle, *gameData);
}

void injectNativeBattleFunctions(engine::scriptingsystem::Lua& script) {
    script.eval("external = {}");

    auto inject = [&](const std::string& name, auto fn) {
        script.registerNativeInNamespace("external", name, fn);
    };

    inject("damage", effects::damage);
    inject("damageWithFixedRecoil", effects::damageWithFixedRecoil);
    inject("damageWithRecoil", effects::damageWithRecoil);
    inject("fixedDamage", effects::fixedDamage);
    inject("lowerStat", effects::lowerStat);
    inject("raiseStat", effects::raiseStat);
    inject("ensureCriticalHit", effects::ensureCriticalHit);
    inject("multiplyDamage", effects::multiplyDamage);
    inject("negateMove", effects::negateMove);
    inject("internalAddStatusCondition", effects::addStatusCondition);
    inject("internalRemoveStatusCondition", effects::removeStatusCondition);
    inject("sleep", effects::sleep);
    inject("reduceSleepCounter", effects::reduceSleepCounter);

    inject("addTimedFlagUser", effects::addTimedFlagUser);
    inject("addFlagUser", effects::addFlagUser);
    inject("addTimedFlagTarget", effects::addTimedFlagTarget);
    inject("addFlagTarget", effects::addFlagTarget);
    inject("removeFlagUser", effects::removeFlagUser);
    inject("removeFlagTarget", effects::removeFlagTarget);
    inject("hasFlag", effects::hasFlag);

    inject("getStatStage", effects::getStatStage);

    inject("random", effects::random);
    inject("getPokemonProperty", effects::getPokemonProperty);
    inject("getMoveProperty", effects::getMoveProperty);
    inject("showText", effects::showText);
}
