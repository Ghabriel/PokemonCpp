#ifndef UTILS_TIMING_CLOCK_HPP
#define UTILS_TIMING_CLOCK_HPP

#include <chrono>

namespace engine::utils {
    /**
     * \brief Simple low-level interface to a clock.
     */
    class Clock {
     public:
        using InternalClock = std::chrono::high_resolution_clock;
        using TimePoint = typename InternalClock::time_point;

        /**
         * \brief Returns the number of milliseconds elapsed since restart()
         * or construction.
         */
        intmax_t getTickCount() const {
            using namespace std::chrono;
            return duration_cast<milliseconds>(now() - startTime).count();
        }

        /**
         * \brief Returns the current time point, according to the internal clock.
         */
        TimePoint now() const {
            return InternalClock::now();
        }

        /**
         * \brief Resets the internal clock to point to the current time point.
         */
        void restart() {
            startTime = now();
        }

     private:
        TimePoint startTime = now();
    };
}

#endif
