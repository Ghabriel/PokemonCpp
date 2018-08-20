#ifndef MOVE_HPP
#define MOVE_HPP

#include <string>

struct Move {
    std::string displayName;
    std::string type;
    std::string kind;
    int functionCode;
    int power;
    int accuracy;
    int pp;
    int effectRate;
    std::string targetType;
    int priority;
    std::string flags;
    std::string description;
};

#endif
