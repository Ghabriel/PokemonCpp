#ifndef EVENT_QUEUE_HPP
#define EVENT_QUEUE_HPP

#include <memory>
#include <queue>

#include "events/Event.hpp"

class EventQueue {
 public:
    void addEvent(std::unique_ptr<Event>);
    void clear();
    bool empty() const;
    void tick();

 private:
    std::queue<std::unique_ptr<Event>> eventQueue;
    std::unique_ptr<Event> currentEvent;
};

#endif
