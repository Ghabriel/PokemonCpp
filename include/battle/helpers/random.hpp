#ifndef RANDOM_HPP
#define RANDOM_HPP

#include <algorithm>
#include <random>

inline int random(int min, int max) {
    static std::random_device randomDevice;
    static std::mt19937 generator(randomDevice());
    std::uniform_int_distribution distribution(min, max);
    return distribution(generator);
}

template<typename RandomIt>
inline void shuffle(RandomIt first, RandomIt last) {
    static std::random_device randomDevice;
    static std::mt19937 generator(randomDevice());
    std::shuffle(first, last, generator);
}

#endif
