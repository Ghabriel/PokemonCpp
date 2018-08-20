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
}

#endif
