#ifndef TESTING_EXPECTATION_HPP
#define TESTING_EXPECTATION_HPP

#include <sstream>
#include "testing-api.hpp"

namespace test {
    template<typename T>
    class Expectation {
     public:
        Expectation(const std::string& file, size_t line, const T& actual)
         : file(file), line(line), actual(actual) {}

        void toBe(const T& expected) const {
            if (actual == expected) {
                success();
                return;
            }

            std::stringstream ss;
            ss << "Expected " << expected << ", got " << actual;
            fail(file, line, ss.str());
        }

     private:
        const std::string& file;
        size_t line;
        const T& actual;
    };

    template<typename T>
    Expectation<T> expect(
        const std::string& file,
        size_t line,
        const T& value
    ) {
        return {file, line, value};
    }
}

#endif
