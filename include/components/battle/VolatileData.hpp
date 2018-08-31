#ifndef VOLATILE_DATA_HPP
#define VOLATILE_DATA_HPP

#include <array>
#include <unordered_set>

struct VolatileData {
    std::array<int, 8> statStages;
    int criticalHitStage;
    std::unordered_set<std::string> flags;
};

#endif
