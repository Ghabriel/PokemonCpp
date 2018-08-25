#ifndef MOVE_EFFECTS_HPP
#define MOVE_EFFECTS_HPP

#include <string>
#include "battle/Stat.hpp"
#include "engine/entity-system/types.hpp"
#include "engine/scripting-system/forward-declarations.hpp"

struct CoreStructures;
struct Move;
struct Pokemon;

namespace effects {
    namespace internal {
        void setGameData(CoreStructures&);
        void setBattle(engine::entitysystem::Entity);
        void setMoveUser(engine::entitysystem::Entity);
        void setMoveTarget(engine::entitysystem::Entity);
        void setMove(Move&);
    }

    void damage();
    void damageWithFixedRecoil(int lostHP);
    void lowerStat(int statId, int levels);
    void raiseStat(int statId, int levels);

    void showText(const std::string& content);
}

void injectNativeBattleFunctions(engine::scriptingsystem::Lua& script);

#endif
