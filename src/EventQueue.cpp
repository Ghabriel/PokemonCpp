#include "EventQueue.hpp"

#include "events/Event.hpp"

void EventQueue::addEvent(std::unique_ptr<Event> event) {
    eventQueue.push(std::move(event));
}

void EventQueue::tick() {
    if (!currentEvent) {
        if (eventQueue.empty()) {
            return;
        }

        currentEvent = std::move(eventQueue.front());
        eventQueue.pop();
        currentEvent->onStart();
    }

    bool finished = currentEvent->tick();

    if (finished) {
        currentEvent = nullptr;
    }
}
