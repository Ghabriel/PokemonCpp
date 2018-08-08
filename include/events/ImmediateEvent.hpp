#ifndef IMMEDIATE_EVENT_HPP
#define IMMEDIATE_EVENT_HPP

#include <functional>
#include "Event.hpp"

class ImmediateEvent : public Event {
 public:
    ImmediateEvent(std::function<void()> fn) : fn(fn) { }

 private:
    std::function<void()> fn;

    void onStartImpl() override {
        fn();
    }

    bool tickImpl() override {
        return true;
    }
};


#endif
