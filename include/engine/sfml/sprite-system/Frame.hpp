#ifndef SPRITE_SYSTEM_FRAME_HPP
#define SPRITE_SYSTEM_FRAME_HPP

namespace engine::spritesystem {
    /**
     * \brief Reprents the portion of a texture that is used to draw an
     * animation frame. Also stores the frame length to allow irregular
     * animations.
     */
    struct Frame {
        int x;
        int y;
        int width;
        int height;
        int lengthInMilliseconds;
    };
}

#endif
