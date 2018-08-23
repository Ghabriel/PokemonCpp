#include "EventQueue.hpp"

#include "engine/utils/debug/xtrace.hpp"

void EventQueue::addEvent(std::unique_ptr<Event> event) {
    eventQueue.push(std::move(event));
}

void EventQueue::clear() {
    eventQueue = {};
}

bool EventQueue::empty() const {
    return !currentEvent && eventQueue.empty();
}

void EventQueue::tick() {
    if (currentEvent) {
        bool finished = currentEvent->tick();

        if (!finished) {
            return;
        }
    }

    if (eventQueue.empty()) {
        currentEvent = nullptr;
        return;
    }

    currentEvent = std::move(eventQueue.front());
    eventQueue.pop();
    currentEvent->onStart();
}
