#ifndef DRAWABLE_VECTOR_HPP
#define DRAWABLE_VECTOR_HPP

#include <SFML/Graphics.hpp>
#include <vector>

struct DrawableVector {
    std::vector<sf::Drawable*> shapes;
};

#endif
