#ifndef STATE_SYSTEM_STATE_MACHINE_HPP
#define STATE_SYSTEM_STATE_MACHINE_HPP

#include <functional>
#include <memory>
#include <stack>
#include <string>
#include <unordered_map>
#include "NullState.hpp"
#include "State.hpp"

namespace engine::statesystem {
    /**
     * \brief Simplifies the application of the %State pattern in games.
     */
    class StateMachine {
     public:
        StateMachine();

        /**
         * \brief Clears the state stack and all registered states. This
         * instance becomes effectively the same as when it was constructed.
         */
        void clear();
        /**
         * \brief Registers a state, mapping it to a given name. Note that
         * this state machine takes ownership of the state.
         */
        void registerState(const std::string& name, std::unique_ptr<State>);
        /**
         * \brief Pushes a state, given its name. Throws if it's not
         * a registered name.
        */
        void pushState(const std::string& stateName);
        /**
         * \brief Pops the current state, returning the state machine to its
         * previous state. Does nothing if there's no current state.
         */
        void popState();
        /**
         * \brief Calls execute() on the current state. Does nothing if
         * there's no current state.
         */
        void execute() const;

     private:
        std::stack<State*> states;
        std::unordered_map<std::string, std::unique_ptr<State>> registeredStates;
    };

    inline StateMachine::StateMachine() {
        registeredStates.insert({"", std::make_unique<NullState>()});
        states.push(registeredStates.at("").get());
    }

    inline void StateMachine::clear() {
        while (states.size() > 1) {
            states.pop();
        }

        auto it = registeredStates.begin();
        while (it != registeredStates.end()) {
            if (it->first != "") {
                it = registeredStates.erase(it);
            } else {
                ++it;
            }
        }
    }

    inline void StateMachine::registerState(
        const std::string& name,
        std::unique_ptr<State> state
    ) {
        registeredStates.insert({name, std::move(state)});
    }

    inline void StateMachine::pushState(const std::string& stateName) {
        states.top()->onExit();
        states.push(registeredStates.at(stateName).get());
        states.top()->onEnter();
    }

    inline void StateMachine::popState() {
        if (states.size() > 1) {
            states.top()->onExit();
            states.pop();
            states.top()->onEnter();
        }
    }

    inline void StateMachine::execute() const {
        states.top()->execute();
    }
}

#endif
