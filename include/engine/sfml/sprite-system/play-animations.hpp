#ifndef SPRITE_SYSTEM_PLAY_ANIMATION_HPP
#define SPRITE_SYSTEM_PLAY_ANIMATION_HPP

#include <SFML/Graphics.hpp>
#include "../../entity-system/include.hpp"
#include "AnimationData.hpp"
#include "AnimationPlaybackData.hpp"
#include "LoopingAnimationData.hpp"

namespace engine::spritesystem {
    /**
     * \brief Advances the animation state of an entity. Should be specialized
     * for each animation data type other than AnimationData and
     * LoopingAnimationData.
     */
    template<typename TAnimationData>
    class AnimationPlayer {
     public:
        void setPlaybackData(AnimationPlaybackData&);
        void setAnimationData(const TAnimationData&);

        /**
         * \brief Checks if the underlying animation has finished
         */
        bool isFinished() const;
        /**
         * \brief Advances the underlying animation state using the
         * elapsed time since the last tick.
         */
        void tick(double timeSinceLastFrame);
        /**
         * \brief Returns the current animation Frame.
         */
        const Frame* getCurrentFrame() const;

     private:
        AnimationPlaybackData* playbackData;
        const TAnimationData* animationData;
    };

    template<typename TAnimationData>
    inline void playAnimations(
        entitysystem::ComponentManager& manager,
        double timeSinceLastFrame
    ) {
        static AnimationPlayer<TAnimationData> animationPlayer;

        manager.forEachEntity<TAnimationData, AnimationPlaybackData>(
            [&](
                entitysystem::Entity entity,
                TAnimationData& animationData,
                AnimationPlaybackData& playbackData
            ) {
                animationPlayer.setPlaybackData(playbackData);
                animationPlayer.setAnimationData(animationData);
                animationPlayer.tick(timeSinceLastFrame);

                if (animationPlayer.isFinished()) {
                    manager.removeComponent<AnimationPlaybackData>(entity);
                    return;
                }

                const Frame* frame = animationPlayer.getCurrentFrame();
                animationData.sprite.setTextureRect(sf::IntRect{
                    frame->x,
                    frame->y,
                    frame->width,
                    frame->height
                });
            }
        );
    }

    template<>
    inline void AnimationPlayer<AnimationData>::setPlaybackData(AnimationPlaybackData& data) {
        playbackData = &data;
    }

    template<>
    inline void AnimationPlayer<AnimationData>::setAnimationData(const AnimationData& data) {
        animationData = &data;
    }

    template<>
    inline bool AnimationPlayer<AnimationData>::isFinished() const {
        return playbackData->currentFrameIndex >= animationData->frames.size();
    }

    template<>
    inline void AnimationPlayer<AnimationData>::tick(double timeSinceLastFrame) {
        if (isFinished()) {
            return;
        }

        auto& currentFrameIndex = playbackData->currentFrameIndex;
        playbackData->currentFrameElapsedTime += timeSinceLastFrame;
        const Frame& currentFrame = animationData->frames.at(currentFrameIndex);

        if (playbackData->currentFrameElapsedTime >= currentFrame.lengthInMilliseconds) {
            ++currentFrameIndex;
            playbackData->currentFrameElapsedTime -= currentFrame.lengthInMilliseconds;
        }
    }

    template<>
    inline const Frame* AnimationPlayer<AnimationData>::getCurrentFrame() const {
        if (isFinished()) {
            return nullptr;
        }

        return &animationData->frames.at(playbackData->currentFrameIndex);
    }

    template<>
    inline void AnimationPlayer<LoopingAnimationData>::setPlaybackData(
        AnimationPlaybackData& data
    ) {
        playbackData = &data;
    }

    template<>
    inline void AnimationPlayer<LoopingAnimationData>::setAnimationData(
        const LoopingAnimationData& data
    ) {
        animationData = &data;
    }

    template<>
    inline bool AnimationPlayer<LoopingAnimationData>::isFinished() const {
        return false;
    }

    template<>
    inline void AnimationPlayer<LoopingAnimationData>::tick(double timeSinceLastFrame) {
        auto& currentFrameIndex = playbackData->currentFrameIndex;
        playbackData->currentFrameElapsedTime += timeSinceLastFrame;
        const Frame& currentFrame = animationData->frames.at(currentFrameIndex);

        if (playbackData->currentFrameElapsedTime >= currentFrame.lengthInMilliseconds) {
            currentFrameIndex = (currentFrameIndex + 1) % animationData->frames.size();
            playbackData->currentFrameElapsedTime -= currentFrame.lengthInMilliseconds;
        }
    }

    template<>
    inline const Frame* AnimationPlayer<LoopingAnimationData>::getCurrentFrame() const {
        return &animationData->frames.at(playbackData->currentFrameIndex);
    }
}

#endif
