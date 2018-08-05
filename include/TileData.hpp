#ifndef TILE_DATA_HPP
#define TILE_DATA_HPP

#include <SFML/Graphics.hpp>
#include <string>

struct TileData {
    std::string texture;
    sf::IntRect rect;
};

#endif
