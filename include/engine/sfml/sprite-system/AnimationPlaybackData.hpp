#ifndef SPRITE_SYSTEM_ANIMATION_PLAYBACK_DATA_HPP
#define SPRITE_SYSTEM_ANIMATION_PLAYBACK_DATA_HPP

namespace engine::spritesystem {
    /**
     * \brief Represents the current progress of an animation. Used as a tag
     * to know which entities are being animated. Note that this component is
     * removed from entities when their animations are completed/stopped.
     */
    struct AnimationPlaybackData {
        size_t currentFrameIndex = 0;
        double currentFrameElapsedTime = 0;
    };
}

#endif
