#ifndef RENDER_HPP
#define RENDER_HPP

#include <SFML/Graphics.hpp>
#include "engine/entity-system/forward-declarations.hpp"
#include "engine/resource-system/forward-declarations.hpp"

void render(
    sf::RenderWindow&,
    engine::entitysystem::ComponentManager&,
    engine::resourcesystem::ResourceStorage&
);

#endif
