#ifndef INPUT_SYSTEM_RAW_INPUT_HPP
#define INPUT_SYSTEM_RAW_INPUT_HPP

#include "KeyboardKey.hpp"

namespace engine::inputsystem {
    /**
     * \brief Interface representing a low-level, platform/framework-dependent
     * input class.
     */
    class RawInput {
     public:
        virtual ~RawInput() = default;

        /**
         * \brief Checks if a given key is currently being pressed.
         */
        bool isKeyPressed(KeyboardKey key) const {
            return isKeyPressedImpl(key);
        }

     private:
        virtual bool isKeyPressedImpl(KeyboardKey) const = 0;
    };
}

#endif
