#ifndef TODO_STATE_HPP
#define TODO_STATE_HPP

#include "../engine/state-system/State.hpp"
#include "../engine/utils/debug/xtrace.hpp"

class TODOState : public engine::statesystem::State {
 private:
    void executeImpl() override {}

    void onEnterImpl() override {
        ECHO("Entering TODOState");
    }

    void onExitImpl() override {
        ECHO("Leaving TODOState");
    }
};

#endif
