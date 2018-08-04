#ifndef RENDER_MAP_HPP
#define RENDER_MAP_HPP

#include <SFML/Graphics.hpp>
#include "engine/entity-system/forward-declarations.hpp"
#include "engine/resource-system/forward-declarations.hpp"

void renderMap(
    sf::RenderWindow&,
    engine::entitysystem::ComponentManager&,
    engine::resourcesystem::ResourceStorage&
);

#endif
