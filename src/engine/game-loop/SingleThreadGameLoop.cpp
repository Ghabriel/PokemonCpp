#include "engine/game-loop/SingleThreadGameLoop.hpp"

using namespace engine::gameloop;

SingleThreadGameLoop::SingleThreadGameLoop(
    std::function<void(SingleThreadGameLoop&, double)> update,
    std::function<void(SingleThreadGameLoop&)> render
) : update(update), render(render) {
    setUpdateFrequency(defaultUpdateFrequency);
}

void SingleThreadGameLoop::setUpdateFrequency(int ticksPerSecond) {
    updatePeriod = 1000.0 / ticksPerSecond;
}

void SingleThreadGameLoop::start() {
    running = true;
    clock.restart();
    spawnGameLoopThread();
}

void SingleThreadGameLoop::stop() {
    running = false;
}

void SingleThreadGameLoop::join() {
    gameLoopThread.join();
}

void SingleThreadGameLoop::spawnGameLoopThread() {
    gameLoopThread = std::thread([&]() {
        double totalTime = 0;
        double currentTime = clock.getTickCount();
        double accumulator = 0;

        while (running) {
            auto now = clock.getTickCount();
            auto elapsedTime = now - currentTime;
            currentTime = now;
            accumulator += elapsedTime;

            while (accumulator >= updatePeriod) {
                update(*this, updatePeriod);
                accumulator -= updatePeriod;
                totalTime += updatePeriod;
            }

            render(*this);
        }
    });
}
