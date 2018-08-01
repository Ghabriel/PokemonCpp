#ifndef SPRITE_SYSTEM_ANIMATION_DATA_HPP
#define SPRITE_SYSTEM_ANIMATION_DATA_HPP

#include <SFML/Graphics.hpp>
#include <vector>
#include "Frame.hpp"

namespace engine::spritesystem {
    /**
     * \brief Represents an animation that happens once and then stops.
     */
    struct AnimationData {
        sf::Sprite sprite;
        std::vector<Frame> frames;
    };
}

#endif
