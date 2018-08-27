#include "battle/events/ValueAnimationEvent.hpp"

#include <cmath>
#include "CoreStructures.hpp"

ValueAnimationEvent::ValueAnimationEvent(
    float& animatedValue,
    float finalValue,
    CoreStructures& gameData
) : animatedValue(&animatedValue),
    initialValue(animatedValue),
    finalValue(finalValue),
    gameData(gameData) { }

void ValueAnimationEvent::onStartImpl() {
    accumulator = 0;
}

bool ValueAnimationEvent::tickImpl() {
    accumulator += *gameData.timeSinceLastFrame;

    if (accumulator >= 16) {
        accumulator = 0;
        int direction = finalValue > *animatedValue ? 1 : -1;
        // float speed = std::sqrt(std::abs(finalValue - *animatedValue)) / (3.0 * 2.23);
        float speed = std::sqrt(std::abs(finalValue - initialValue)) / 6.7;
        *animatedValue += speed * direction;

        int newDirection = finalValue > *animatedValue ? 1 : -1;
        if (newDirection != direction) {
            *animatedValue = finalValue;
        }

        if (std::abs(finalValue - *animatedValue) < 0.1) {
            *animatedValue = finalValue;
            return true;
        }
    }

    return false;
}
