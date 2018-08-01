#ifndef UTILS_TIMING_FREQUENCY_GAUGE_HPP
#define UTILS_TIMING_FREQUENCY_GAUGE_HPP

#include "Clock.hpp"

namespace engine::utils {
    /**
     * \brief Simplifies frequency-related calculations.
     * Can be used e.g to measure the FPS count of the game.
     */
    class FrequencyGauge {
    public:
        /**
         * \brief Resets the internal counter.
         */
        void reset() {
            counter = 0;
            clock.restart();
        }

        /**
         * \brief Increases the internal counter.
         */
        void tick() {
            ++counter;
        }

        /**
         * \brief Retrieves the internal counter.
         */
        intmax_t tickCount() const {
            return counter;
        }

        /**
         * \brief Measures the tick frequency (in ticks per second) since
         * reset() or construction.
         */
        long double measure() const {
            return counter * 1000.0L / clock.getTickCount();
        }

    private:
        Clock clock;
        intmax_t counter = 0;
    };
}

#endif
