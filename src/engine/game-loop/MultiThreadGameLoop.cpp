#include "engine/game-loop/MultiThreadGameLoop.hpp"

using namespace engine::gameloop;

MultiThreadGameLoop::MultiThreadGameLoop(
    std::function<void(MultiThreadGameLoop&)> update,
    std::function<void(MultiThreadGameLoop&, double)> render
) : update(update), render(render) {
    setUpdateFrequency(defaultUpdateFrequency);
}

void MultiThreadGameLoop::setUpdateFrequency(int ticksPerSecond) {
    updatePeriod = 1000 / ticksPerSecond;
}

void MultiThreadGameLoop::start() {
    running = true;
    clock.restart();
    spawnUpdateThread();
    spawnRenderThread();
}

void MultiThreadGameLoop::stop() {
    running = false;
}

void MultiThreadGameLoop::join() {
    updateThread.join();
    renderThread.join();
}

void MultiThreadGameLoop::spawnUpdateThread() {
    updateThread = std::thread([&]() {
        nextUpdate = clock.getTickCount();

        while (running) {
            auto now = clock.getTickCount();

            if (now >= nextUpdate) {
                update(*this);
                nextUpdate += updatePeriod;
            }
        }
    });
}

void MultiThreadGameLoop::spawnRenderThread() {
    renderThread = std::thread([&]() {
        while (running) {
            auto now = clock.getTickCount();
            double timeSinceUpdate = now + updatePeriod - nextUpdate;

            if (timeSinceUpdate > updatePeriod) {
                // Update is being processed, assume that it already happened
                timeSinceUpdate -= updatePeriod;
            }

            double interpolation = timeSinceUpdate / updatePeriod;
            render(*this, interpolation);
        }
    });
}
