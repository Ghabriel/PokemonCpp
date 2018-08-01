#ifndef STATE_SYSTEM_NULL_STATE_HPP
#define STATE_SYSTEM_NULL_STATE_HPP

#include "State.hpp"

namespace engine::statesystem {
    /**
     * \brief A simple State that does nothing when executed.
     */
    class NullState : public State {
     private:
        void executeImpl() override { }
        void onEnterImpl() override { }
        void onExitImpl() override { }
    };
}

#endif
