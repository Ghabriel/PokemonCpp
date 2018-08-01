#ifndef INPUT_SYSTEM_INPUT_CONTEXT_HPP
#define INPUT_SYSTEM_INPUT_CONTEXT_HPP

#include <functional>
#include <unordered_map>
#include "types.hpp"

namespace engine::inputsystem {
    /**
     * \brief Represents a group of mappings between contextualized keyboard
     * keys (game keys) and their callbacks.
     *
     * There are two types of input commands:
     * * **actions**: commands that trigger only once regardless if the player
     * keeps the key pressed. Ideal for commands like selecting a menu option;
     * * **states**: commands that are repeatedly triggered for as long as the
     * player keeps the key pressed. Ideal for commands like moving a character.
     *
     * Additionally, input contexts have a **priority**. There might be multiple
     * active input contexts at the same time in a game (for example, consider
     * a menu that intercepts some keyboard actions but not all of them), and
     * in that situation the priority controls which context(s) receive the
     * input.
     */
    struct InputContext {
        std::unordered_map<GameKey, std::function<void()>> actions;
        std::unordered_map<GameKey, std::function<void()>> states;
        size_t priority;
    };
}

#endif
