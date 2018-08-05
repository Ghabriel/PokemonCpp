#ifndef TILE_HPP
#define TILE_HPP

#include <SFML/Graphics.hpp>

struct Tile {
    // ordered by layer
    std::vector<sf::Sprite> sprites;
};

#endif
