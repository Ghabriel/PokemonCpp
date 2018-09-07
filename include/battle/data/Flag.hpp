#ifndef FLAG_HPP
#define FLAG_HPP

#include <string>
#include "../../engine/entity-system/types.hpp"

struct Flag {
    engine::entitysystem::Entity target;
    std::string id;
    int duration;
    int data;
};

#endif
