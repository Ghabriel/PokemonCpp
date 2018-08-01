#ifndef TESTING_API_HPP
#define TESTING_API_HPP

#include <functional>

namespace test {
    using Name = std::string;
    using Function = std::function<void()>;
    using File = const std::string&;
    using Line = size_t;
    using Message = const std::string&;

    void describe(const Name&, Function);
    void it(const Name&, Function);
    void before(Function);
    void success();
    void fail(File, Line, Message);
}

#endif
