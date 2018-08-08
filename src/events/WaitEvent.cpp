#include "events/WaitEvent.hpp"

#include "CoreStructures.hpp"

WaitEvent::WaitEvent(int ms, CoreStructures& gameData)
 : ms(ms), gameData(gameData) { }

void WaitEvent::onStartImpl() {
    accumulator = 0;
}

bool WaitEvent::tickImpl() {
    accumulator += *gameData.timeSinceLastFrame;
    return accumulator >= ms;
}
