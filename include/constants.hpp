#ifndef CONSTANTS_HPP
#define CONSTANTS_HPP

#include <array>

namespace constants {
    constexpr int MOVE_LIMIT = 4;
    constexpr std::array<const char*, 8> DISPLAYNAME_STATS = {
        "HP",
        "ATTACK",
        "DEFENSE",
        "SP.ATK",
        "SP.DEF",
        "SPEED",
        "ACCURACY",
        "EVASION"
    };
}

#endif
