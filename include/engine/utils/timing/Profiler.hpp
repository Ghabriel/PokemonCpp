#ifndef UTILS_TIMING_PROFILER_HPP
#define UTILS_TIMING_PROFILER_HPP

#include <chrono>

namespace engine::utils {
    /**
     * \brief Provides a simple way to profile arbitrary code.
     */
    class Profiler {
     public:
        /**
         * \brief Starts a time measurement.
         */
        void start() {
            startTime = std::chrono::steady_clock::now();
        }

        /**
         * \brief Finishes a time measurement. The elapsed time can be obtained via
         * measure(), measureAsSeconds(), measureAsMilliseconds() or
         * measureAsMicroseconds().
         */
        void finish() {
            finishTime = std::chrono::steady_clock::now();
        }

        /**
         * \brief Returns the elapsed time in an arbitrary time unit.
         */
        template<typename T>
        intmax_t measure() const {
            return std::chrono::duration_cast<T>(finishTime - startTime).count();
        }

        /**
         * \brief Returns the elapsed time in seconds, rounded down to
         * an integer.
         */
        intmax_t measureAsSeconds() const {
            return measure<std::chrono::seconds>();
        }

        /**
         * \brief Returns the elapsed time in milliseconds, rounded down to
         * an integer.
         */
        intmax_t measureAsMilliseconds() const {
            return measure<std::chrono::milliseconds>();
        }

        /**
         * \brief Returns the elapsed time in microseconds, rounded down to
         * an integer.
         */
        intmax_t measureAsMicroseconds() const {
            return measure<std::chrono::microseconds>();
        }

     private:
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point finishTime;
    };
}

#endif
