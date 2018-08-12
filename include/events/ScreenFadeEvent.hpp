#ifndef SCREEN_FADE_EVENT_HPP
#define SCREEN_FADE_EVENT_HPP

#include <SFML/Graphics.hpp>
#include "../engine/entity-system/types.hpp"
#include "Event.hpp"

struct CoreStructures;

class ScreenFadeEvent : public Event {
    using Entity = engine::entitysystem::Entity;
 public:
    ScreenFadeEvent(Entity map, CoreStructures& gameData);

 private:
    sf::RectangleShape topRect;
    sf::RectangleShape bottomRect;
    Entity map;
    CoreStructures& gameData;

    void onStartImpl() override;
    bool tickImpl() override;
};


#endif
