#ifndef WAIT_EVENT_HPP
#define WAIT_EVENT_HPP

#include "Event.hpp"

struct CoreStructures;

class WaitEvent : public Event {
 public:
    WaitEvent(int ms, CoreStructures& gameData);

 private:
    int ms;
    CoreStructures& gameData;
    float accumulator;

    void onStartImpl() override;
    bool tickImpl() override;
};


#endif
