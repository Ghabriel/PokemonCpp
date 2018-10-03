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

int effects::getStandardStat(int entityId, int statId) {
    Entity entity = getPokemonEntity(entityId);
    const Pokemon& pokemon = data<Pokemon>(entity, *gameData);
    return pokemon.stats[statId];
}

int effects::getStatStage(int entityId, int statId) {
    Entity entity = getPokemonEntity(entityId);
    return data<VolatileData>(entity, *gameData).statStages[statId];
}


int effects::random(int min, int max) {
    return ::random(min, max);
}

void effects::showText(const std::string& content) {
    enqueueEvent<TextEvent>(content, battle, *gameData);
}

void effects::pushPokemonProperty(int entityId, const std::string& property) {
    Entity pokemonEntity = getPokemonEntity(entityId);
    const Pokemon& currentPokemon = data<Pokemon>(pokemonEntity, *gameData);

    const auto setValue = [&](const auto& value) {
        script("moves", *gameData).set("external.dataRegister", value);
    };

    for (size_t i = 0; i < constants::MOVE_LIMIT; ++i) {
        if (property == "move" + std::to_string(i)) {
            setValue((i < currentPokemon.moves.size()) ? currentPokemon.moves[i] : "");
            return;
        }

        if (property == "pp" + std::to_string(i)) {
            setValue((i < currentPokemon.pp.size()) ? currentPokemon.pp[i] : -1);
            return;
        }
    }

    const auto getStat = [&](Stat stat) {
        return lua::call<int>("getEffectiveStat", pokemonEntity, stat, StatFlags::All);
    };

    PokemonSpeciesData& speciesData = data<PokemonSpeciesData>(pokemonEntity, *gameData);
    size_t typeCount = speciesData.types.size();

    if (property == "species") {
        setValue(currentPokemon.species);
    } else if (property == "nature") {
        setValue(static_cast<int>(currentPokemon.nature));
    } else if (property == "heldItem") {
        setValue(currentPokemon.heldItem);
    } else if (property == "experiencePoints") {
        setValue(currentPokemon.experiencePoints);
    } else if (property == "ability") {
        setValue(currentPokemon.ability);
    } else if (property == "moveCount") {
        setValue(static_cast<int>(currentPokemon.moves.size()));
    } else if (property == "gender") {
        setValue(static_cast<int>(currentPokemon.gender));
    } else if (property == "form") {
        setValue(currentPokemon.form);
    } else if (property == "displayName") {
        setValue(currentPokemon.displayName);
    } else if (property == "pokerus") {
        setValue(currentPokemon.pokerus);
    } else if (property == "statusCondition") {
        setValue(static_cast<int>(currentPokemon.status));
    } else if (property == "asleepRounds") {
        setValue(currentPokemon.asleepRounds);
    } else if (property == "level") {
        setValue(currentPokemon.level);
    } else if (property == "hp") {
        setValue(getStat(Stat::HP));
    } else if (property == "attack") {
        setValue(getStat(Stat::Attack));
    } else if (property == "defense") {
        setValue(getStat(Stat::Defense));
    } else if (property == "specialAttack") {
        setValue(getStat(Stat::SpecialAttack));
    } else if (property == "specialDefense") {
        setValue(getStat(Stat::SpecialDefense));
    } else if (property == "speed") {
        setValue(getStat(Stat::Speed));
    } else if (property == "currentHP") {
        setValue(static_cast<int>(currentPokemon.currentHP));
    } else if (property == "typeCount") {
        setValue(static_cast<int>(typeCount));
    } else if (property == "type0") {
        setValue(speciesData.types[0]);
    } else if (property == "type1") {
        setValue((typeCount == 1 ? "" : speciesData.types[1]));
    } else if (property == "baseExp") {
        setValue(speciesData.baseExp);
    } else if (property == "captureRate") {
        setValue(speciesData.captureRate);
    } else if (property == "height") {
        setValue(speciesData.height);
    } else if (property == "weight") {
        setValue(speciesData.weight);
    } else {
        assert(false);
    }
}

void effects::pushMoveProperty(int pokemonId, int moveIndex, const std::string& property) {
    BoundMove requestedMove = getBoundMove(pokemonId, moveIndex);
    const Pokemon& moveUser = data<Pokemon>(requestedMove.user, *gameData);
    int currentPP = moveUser.pp[requestedMove.moveIndex];

    const auto setValue = [&](const auto& value) {
        script("moves", *gameData).set("external.dataRegister", value);
    };

    if (property == "displayName") {
        setValue(move->displayName);
    } else if (property == "type") {
        setValue(move->type);
    } else if (property == "kind") {
        setValue(move->kind);
    } else if (property == "power") {
        setValue(move->power);
    } else if (property == "accuracy") {
        setValue(move->accuracy);
    } else if (property == "currentPP") {
        setValue(currentPP);
    } else if (property == "targetType") {
        setValue(move->targetType);
    } else if (property == "priority") {
        setValue(move->priority);
    } else if (property == "flags") {
        setValue(move->flags);
    } else {
        assert(false);
    }
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
    inject("addStatusCondition", effects::addStatusCondition);
    inject("removeStatusCondition", effects::removeStatusCondition);
    inject("sleep", effects::sleep);
    inject("reduceSleepCounter", effects::reduceSleepCounter);

    inject("addTimedFlagUser", effects::addTimedFlagUser);
    inject("addFlagUser", effects::addFlagUser);
    inject("addTimedFlagTarget", effects::addTimedFlagTarget);
    inject("addFlagTarget", effects::addFlagTarget);
    inject("removeFlagUser", effects::removeFlagUser);
    inject("removeFlagTarget", effects::removeFlagTarget);
    inject("hasFlag", effects::hasFlag);

    inject("getStandardStat", effects::getStandardStat);
    inject("getStatStage", effects::getStatStage);

    inject("random", effects::random);
    inject("pushPokemonProperty", effects::pushPokemonProperty);
    inject("pushMoveProperty", effects::pushMoveProperty);
    inject("showText", effects::showText);
}
