#ifndef MOVE_EFFECTS_HPP
#define MOVE_EFFECTS_HPP

#include <functional>
#include <string>
#include "battle/data/Stat.hpp"
#include "engine/entity-system/types.hpp"
#include "engine/scripting-system/forward-declarations.hpp"

struct BoundMove;
struct CoreStructures;
class EventManager;
struct Flag;
struct Move;
struct Pokemon;

namespace effects {
    namespace internal {
        void setGameData(CoreStructures&);
        void setBattle(engine::entitysystem::Entity);
        void setEventManager(EventManager&);
        void setMove(const BoundMove&);
        void setFlag(const Flag&);
    }

    // used by C++ only
    void cleanup();
    bool isMoveNegated();

    void damage();
    void damageWithFixedRecoil(int lostHP);
    void damageWithRecoil(float recoilRate);
    void fixedDamage(int lostHP);
    void lowerStat(int statId, int levels);
    void raiseStat(int statId, int levels);
    void ensureCriticalHit();
    void multiplyDamage(float factor);
    void negateMove();
    void addStatusCondition(int entityId, int statusConditionId);
    void removeStatusCondition(int entityId);
    void sleep(int entityId, int duration);
    void reduceSleepCounter();

    // Flag-related functions
    void addTimedFlagUser(const std::string& flagId, int duration);
    void addFlagUser(const std::string& flagId);
    void addTimedFlagTarget(const std::string& flagId, int duration);
    void addFlagTarget(const std::string& flagId);
    void removeFlagUser(const std::string& flagId);
    void removeFlagTarget(const std::string& flagId);
    bool hasFlag(int entityId, const std::string& flagId);

    // Getters
    int getStandardStat(int entityId, int statId);
    int getStatStage(int entityId, int statId);

    int random(int min, int max);
    void showText(const std::string& content);
    void pushPokemonProperty(int entityId, const std::string& property);
    void pushMoveProperty(int pokemonId, int moveIndex, const std::string& property);
}

void injectNativeBattleFunctions(engine::scriptingsystem::Lua& script);

#endif
