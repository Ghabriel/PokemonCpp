#ifndef PLAYER_TEXT_EVENT_HPP
#define PLAYER_TEXT_EVENT_HPP

#include <string>
#include "../engine/entity-system/types.hpp"
#include "Event.hpp"

struct CoreStructures;

class TextEvent : public Event {
    using Entity = engine::entitysystem::Entity;
 public:
    TextEvent(const std::string& content, Entity map, CoreStructures& gameData);

 private:
    static constexpr int charDelayMs = 30;
    std::string content;
    char nextChar;
    float accumulator;
    Entity map;
    CoreStructures& gameData;

    void onStartImpl() override;
    bool tickImpl() override;
};


#endif
