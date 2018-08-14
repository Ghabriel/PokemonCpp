#ifndef RENDER_BATTLE_HPP
#define RENDER_BATTLE_HPP

#include <SFML/Graphics.hpp>
#include "../engine/entity-system/forward-declarations.hpp"
#include "../engine/resource-system/forward-declarations.hpp"

void renderBattle(
    sf::RenderWindow&,
    engine::entitysystem::ComponentManager&,
    engine::resourcesystem::ResourceStorage&
);

#endif
