#ifndef VOLATILE_DATA_HPP
#define VOLATILE_DATA_HPP

#include <array>

struct VolatileData {
    std::array<int, 8> statStages;
    int criticalHitStage;
};

#endif
