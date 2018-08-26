#ifndef VALUE_ANIMATION_EVENT_HPP
#define VALUE_ANIMATION_EVENT_HPP

#include "../../events/Event.hpp"

struct CoreStructures;

class ValueAnimationEvent : public Event {
 public:
    ValueAnimationEvent(
        float& animatedValue,
        float finalValue,
        CoreStructures& gameData
    );

 private:
    float* animatedValue;
    float initialValue;
    float finalValue;
    CoreStructures& gameData;
    float accumulator;

    void onStartImpl() override;
    bool tickImpl() override;
};


#endif
