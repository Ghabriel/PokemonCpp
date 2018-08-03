#ifndef UTILS_MISC_MENU_HPP
#define UTILS_MISC_MENU_HPP

#include <functional>
#include <vector>
#include "../../state-system/StateMachine.hpp"

namespace engine::utils {
    /**
     * \brief Represents a simple, generic menu. Game-specific code can wrap this
     * class in a State and provide specific highlight methods (e.g move up, move
     * down) using focus() as their base.
     */
    class Menu {
        using StateMachine = engine::statesystem::StateMachine;
     public:
        Menu(StateMachine&);

        /**
         * \brief Adds an option to this menu, returning its index. Note that
         * the target state doesn't need to be registered in the state machine
         * until it is selected.
         */
        size_t addOption(const std::string& label, const std::string& stateName);

        /**
         * \brief Returns the number of options in this menu.
         */
        size_t size() const;

        /**
         * \brief Returns the index of the currently focused option.
         */
        size_t getFocusedOption() const;

        /**
         * \brief Focuses a specific option by its index.
         */
        void focus(size_t index);

        /**
         * \brief Selects the focused option, triggering a state change. The
         * target state must be registered in the state machine.
         */
        void select() const;

        /**
         * \brief Executes a callback function for every label in this menu.
         * `fn` must have an `operator()` compatible with
         * `operator()(const std::string&)`.
         */
        template<typename Functor>
        void forEachLabel(Functor fn) const;

     private:
        std::vector<std::string> labels;
        std::vector<std::string> targetStates;
        StateMachine& stateMachine;
        size_t focusedIndex = 0;
    };

    inline Menu::Menu(StateMachine& machine) : stateMachine(machine) { }

    inline size_t Menu::addOption(const std::string& label, const std::string& stateName) {
        labels.emplace_back(label);
        targetStates.emplace_back(stateName);
        return labels.size() - 1;
    }

    inline size_t Menu::size() const {
        return targetStates.size();
    }

    inline size_t Menu::getFocusedOption() const {
        return focusedIndex;
    }

    inline void Menu::focus(size_t index) {
        focusedIndex = index;
    }

    inline void Menu::select() const {
        // Avoids a possible reference invalidation when the current
        // state's onExit() causes the destruction of this Menu.
        auto stateNameCopy = targetStates.at(focusedIndex);
        stateMachine.pushState(stateNameCopy);
    }

    template<typename Functor>
    inline void Menu::forEachLabel(Functor fn) const {
        for (const auto& label : labels) {
            fn(label);
        }
    }
}

#endif
