#ifndef SPRITE_SYSTEM_LOOPING_ANIMATION_DATA_HPP
#define SPRITE_SYSTEM_LOOPING_ANIMATION_DATA_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Frame.hpp"

namespace engine::spritesystem {
    /**
     * \brief Represents an animation that loops indefinitely.
     */
    struct LoopingAnimationData {
        sf::Sprite sprite;
        std::vector<Frame> frames;
    };
}

#endif
