#ifndef MOVE_EFFECTS_HPP
#define MOVE_EFFECTS_HPP

#include <string>
#include "engine/entity-system/types.hpp"
#include "engine/scripting-system/forward-declarations.hpp"

struct CoreStructures;
struct Move;
struct Pokemon;

namespace effects {
    namespace internal {
        void setGameData(CoreStructures&);
        void setBattle(engine::entitysystem::Entity);
        void setMoveUser(Pokemon&);
        void setMoveTarget(Pokemon&);
        void setMove(Move&);
    }

    void damage();
    void lowerStat(int stat, int levels);
    void raiseStat(int stat, int levels);

    void showText(const std::string& content);
}

void injectNativeBattleFunctions(engine::scriptingsystem::Lua& script);

#endif
