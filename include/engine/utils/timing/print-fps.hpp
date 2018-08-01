#ifndef UTILS_TIMING_PRINT_FPS_HPP
#define UTILS_TIMING_PRINT_FPS_HPP

#include <iomanip>
#include <iostream>
#include <limits>
#include "FrequencyGauge.hpp"

namespace engine::utils {
    template<int CounterId>
    void printFPS(const std::string& message, intmax_t frequency) {
        static FrequencyGauge gauge;

        gauge.tick();

        if (gauge.tickCount() % frequency == 0) {
            auto fps = gauge.measure();
            constexpr auto precision = std::numeric_limits<long double>::digits10 + 1;
            std::cout << std::setprecision(precision) << message << ": " << fps << '\n';
            gauge.reset();
        }
    }
}

#endif
