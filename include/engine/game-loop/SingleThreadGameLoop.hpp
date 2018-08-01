#ifndef SINGLE_THREAD_GAME_LOOP_HPP
#define SINGLE_THREAD_GAME_LOOP_HPP

#include <functional>
#include <thread>
#include "../utils/timing/Clock.hpp"

namespace engine::gameloop {
    /**
     * \brief Manages a single-threaded game loop and provides control
     * mechanisms to it.
     */
    class SingleThreadGameLoop {
        using Clock = utils::Clock;
     public:
        SingleThreadGameLoop(
            std::function<void(SingleThreadGameLoop&, double)> update,
            std::function<void(SingleThreadGameLoop&)> render
        );

        SingleThreadGameLoop(SingleThreadGameLoop&) = delete;
        SingleThreadGameLoop(SingleThreadGameLoop&&) = delete;
        SingleThreadGameLoop& operator=(SingleThreadGameLoop&) = delete;
        SingleThreadGameLoop& operator=(SingleThreadGameLoop&&) = delete;

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
        std::function<void(SingleThreadGameLoop&, double)> update;
        std::function<void(SingleThreadGameLoop&)> render;
        double updatePeriod;

        std::thread gameLoopThread;
        bool running = false;
        Clock clock;

        void spawnGameLoopThread();
    };
}

#endif
