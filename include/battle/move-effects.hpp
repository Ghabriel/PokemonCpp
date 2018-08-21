#ifndef MOVE_EFFECTS_HPP
#define MOVE_EFFECTS_HPP

struct CoreStructures;
struct Move;
struct Pokemon;

namespace effects {
    namespace internal {
        void setGameData(CoreStructures&);
        void setMoveUser(Pokemon&);
        void setMoveTarget(Pokemon&);
        void setMove(Move&);
    }

    void damage();
    void lowerStat(int stat, int levels);
    void raiseStat(int stat, int levels);
}

#endif
