#ifndef RAW_INPUT_SFML_HPP
#define RAW_INPUT_SFML_HPP

#include <SFML/Window/Keyboard.hpp>
#include "engine/input-system/RawInput.hpp"

class RawInputSFML : public engine::inputsystem::RawInput {
    bool isKeyPressedImpl(engine::inputsystem::KeyboardKey key) const override {
        return sf::Keyboard::isKeyPressed(
            static_cast<sf::Keyboard::Key>(key)
        );
    }
};

#endif
