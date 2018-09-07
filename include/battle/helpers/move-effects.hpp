#ifndef MOVE_EFFECTS_HPP
#define MOVE_EFFECTS_HPP

#include <functional>
#include <string>
#include "battle/data/Stat.hpp"
#include "engine/entity-system/types.hpp"
#include "engine/scripting-system/forward-declarations.hpp"

struct BoundMove;
struct CoreStructures;
struct Flag;
struct Move;
struct Pokemon;

namespace effects {
    namespace internal {
        void setGameData(CoreStructures&);
        void setBattle(engine::entitysystem::Entity);
        void setTriggerEvent(std::function<void(const std::string& eventName)>);
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
    // TODO: reimplement addFlag/removeFlag/etc
    void multiplyDamage(float factor);
    void negateMove();

    void showText(const std::string& content);
}

void injectNativeBattleFunctions(engine::scriptingsystem::Lua& script);

#endif
