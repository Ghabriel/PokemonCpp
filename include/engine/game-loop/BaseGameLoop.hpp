#ifndef GAME_LOOP_HPP
#define GAME_LOOP_HPP

#include <atomic>
#include <functional>
#include <thread>
#include "../utils/timing/Clock.hpp"

namespace engine::gameloop {
    /**
     * \brief Manages a multi-threaded game loop and provides control
     * mechanisms to it.
     */
    class MultiThreadGameLoop {
        using Clock = utils::Clock;
     public:
        MultiThreadGameLoop(
            std::function<void(MultiThreadGameLoop&)> update,
            std::function<void(MultiThreadGameLoop&, double)> render
        );

        MultiThreadGameLoop(MultiThreadGameLoop&) = delete;
        MultiThreadGameLoop(MultiThreadGameLoop&&) = delete;
        MultiThreadGameLoop& operator=(MultiThreadGameLoop&) = delete;
        MultiThreadGameLoop& operator=(MultiThreadGameLoop&&) = delete;

        /**
         * \brief Changes the frequency at which update calls are fired.
         * Note that the update frequency is the **game speed**, not the FPS.
         */
        void setUpdateFrequency(int ticksPerSecond);

        /**
         * \brief Starts the game. Two threads are created, one for updates
         * (game logic) and one for rendering. The update frequency can be
         * changed via setUpdateFrequency() and defaults to 25 ticks per second.
         * The render frequency is unbounded.
         */
        void start();
        /**
         * \brief Stops the game. Both the update thread and the render thread
         * are stopped as soon as their current iterations are completed, if any.
         */
        void stop();
        /**
         * \brief Calls join() on both the update thread and the render thread.
         */
        void join();

     private:
        static constexpr int defaultUpdateFrequency = 25;
        std::function<void(MultiThreadGameLoop&)> update;
        std::function<void(MultiThreadGameLoop&, double)> render;
        int updatePeriod;

        std::thread updateThread;
        std::thread renderThread;
        bool running = false;
        Clock clock;
        std::atomic<intmax_t> nextUpdate;

        void spawnUpdateThread();
        void spawnRenderThread();
    };
}

#endif
