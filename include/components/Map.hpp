#ifndef MAP_HPP
#define MAP_HPP

#include <cstddef>
#include <string>
#include <vector>
#include "Tile.hpp"

struct Map {
    size_t id;
    std::string name;
    size_t widthInTiles;
    size_t heightInTiles;
    std::vector<Tile> tiles;
};

#endif
