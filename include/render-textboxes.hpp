#ifndef RENDER_TEXT_BOXES_HPP
#define RENDER_TEXT_BOXES_HPP

#include <SFML/Graphics.hpp>
#include "engine/entity-system/forward-declarations.hpp"
#include "engine/resource-system/forward-declarations.hpp"

void renderTextBoxes(
    sf::RenderWindow&,
    engine::entitysystem::ComponentManager&,
    engine::resourcesystem::ResourceStorage&
);

#endif
