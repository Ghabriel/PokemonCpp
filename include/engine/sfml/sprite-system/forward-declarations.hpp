#include "../entity-system/forward-declarations.hpp"

namespace engine::spritesystem {
    struct AnimationData;
    struct AnimationPlaybackData;
    struct LoopingAnimationData;
    template<typename TAnimationData>
    class AnimationPlayer;
    template<typename TAnimationData>
    void playAnimations(entitysystem::ComponentManager&);
}
