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
    static constexpr int charDelayMs = 16; // rounded to the next multiple of 16.66
    std::string content;
    size_t nextCharIndex;
    float timeAccumulator;
    Entity map;
    CoreStructures& gameData;

    void onStartImpl() override;
    bool tickImpl() override;
    void onClose();
};


#endif
