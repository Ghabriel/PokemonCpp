#include "engine/testing/testing-api.hpp"

#include <iostream>
#include <sstream>
#include <vector>
#include "engine/testing/Color.hpp"

using test::Name;
using test::Function;
using test::File;
using test::Line;
using test::Message;

namespace {
    Name currentSpec;
    Name currentTestCase;
    size_t successCount = 0;
    size_t failCount = 0;
    std::vector<Function> beforeCallbacks;
    std::stringstream buffer;

    void callBefore() {
        for (const auto& callback : beforeCallbacks) {
            callback();
        }
    }
}

void test::describe(const Name& name, Function body) {
    currentSpec = name;
    beforeCallbacks.clear();
    std::cout << "[" << name << "]" << std::endl;
    body();
}

void test::it(const Name& name, Function body) {
    currentTestCase = name;

    successCount = 0;
    failCount = 0;
    buffer.str("");

    callBefore();
    body();

    std::stringstream stats;
    stats << successCount << "/" << (successCount + failCount);

    FontColor statsColor = (failCount == 0) ? FontColor::Green : FontColor::Red;

    std::cout << "  [" << name << "] ";
    std::cout << applyColor(stats.str(), statsColor, FontDecoration::Bold);
    std::cout << std::endl;

    std::cout << buffer.str();
}

void test::before(Function body) {
    beforeCallbacks.push_back(body);
}

void test::success() {
    successCount++;
}

void test::fail(File file, Line line, Message message) {
    failCount++;
    buffer << "    ";
    buffer << applyColor(
        "(" + file + ":" + std::to_string(line) + ") ",
        FontColor::Yellow,
        FontDecoration::Bold
    );
    buffer << applyColor(message, FontColor::Red, FontDecoration::Bold) << std::endl;
}
