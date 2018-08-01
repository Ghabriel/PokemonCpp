/* created by Ghabriel Nunes <ghabriel.nunes@gmail.com> [2017] */
#ifndef UTILS_DEBUG_HPP
#define UTILS_DEBUG_HPP

#include <csignal>
#include <functional>
#include <iostream>
#include <sstream>
#include <tuple>
#include <unordered_map>
#include <utility>

#ifndef ALLOW_DEBUG_USAGE
#define ALLOW_DEBUG_USAGE 1
#endif

#ifndef DEBUG_ENABLED
#define DEBUG_ENABLED 1
#endif

#ifndef FUNCTION_NAME
#define FUNCTION_NAME __func__
#endif

#if ALLOW_DEBUG_USAGE == 0
#define FN_SUFFIX __attribute__((deprecated("debug usage")))
#else
#define FN_SUFFIX
#endif

namespace dbg {
    template<typename... Args>
    void echo(Args&&...) FN_SUFFIX;

    template<typename... Args>
    inline void echoIndented(size_t numTabs, Args&&... args) FN_SUFFIX;

    template<typename... Args>
    inline void trace(Args&&... args) FN_SUFFIX;

    template<typename T>
    inline void traceIterable(const std::string& name, const T& value) FN_SUFFIX;

    inline void debug(size_t line, const std::string& filename, const std::string& fn) FN_SUFFIX;

    template<typename T>
    inline void redirect(T& stream) FN_SUFFIX;

    template<typename T>
    inline void redirect(const T& stream) FN_SUFFIX;

    namespace __detail {
        using DebugBufferType = std::tuple<size_t, std::string, std::string>;
        inline DebugBufferType& debugBuffer() {
            static DebugBufferType buffer;
            return buffer;
        };

        const std::unordered_map<int, std::string> debugLabels = {
            {SIGABRT, "Aborted"},
            {SIGFPE, "Division by zero"},
            {SIGILL, "SIGILL"},
            {SIGINT, "Interruption"},
            {SIGSEGV, "Segmentation fault"},
            {SIGTERM, "SIGTERM"}
        };

        inline void printer(int type) {
            #if ALLOW_DEBUG_USAGE == 1
                auto& buffer = debugBuffer();
                auto& line = std::get<0>(buffer);
                auto& filename = std::get<1>(buffer);
                auto& functionName = std::get<2>(buffer);
                std::stringstream ss;
                ss << debugLabels.at(type) << ".\n  Location: " << filename
                   << ":" << line << "\n  Function: " << functionName;
                echo(ss.str());
                std::signal(type, SIG_DFL);
                std::raise(type);
            #endif
        }

        using Redirector = std::function<void(const std::string&)>;
        using StreamType = decltype(std::cout);

        struct StreamContainer {
            static StreamContainer& instance() {
                static StreamContainer inst;
                return inst;
            }

            StreamContainer& operator<<(const std::string& message) {
                redirector(message);
                return *this;
            }

            Redirector redirector = [](const std::string& message) {
                StreamContainer::instance().stream.get() << message;
            };

            std::reference_wrapper<StreamType> stream = std::cout;

         private:
            StreamContainer() = default;
        };

        const auto defaultRedirector = StreamContainer::instance().redirector;

        inline StreamContainer& stream() {
            return StreamContainer::instance();
        }

        template<bool = DEBUG_ENABLED, bool = ALLOW_DEBUG_USAGE>
        struct DebugContainer {
            template<typename... Args>
            static void echo(Args&&...) {}

            template<typename... Args>
            static void echoIndented(Args&&...) {}

            template<typename... Args>
            static void trace(Args&&...) {}

            template<typename T>
            static void traceIterable(const std::string&, const T&) {}

            static void debug(size_t, const std::string&, const std::string&) {}

            template<typename T>
            static void redirect(const T&) {}
        };

        template<>
        struct DebugContainer<true, true> {
            static void echo() {}

            template<typename T, typename... Args>
            static void echo(const T& value, Args&&... args) {
                std::stringstream ss;
                ss << value << '\n';
                stream() << ss.str();
                echo(std::forward<Args>(args)...);
            }

            template<typename... Args>
            static void echo(const std::string& value, Args&&... args) {
                stream() << (value + '\n');
                echo(std::forward<Args>(args)...);
            }

            static void echoIndented(size_t) {}

            template<typename T, typename... Args>
            static void echoIndented(size_t numTabs, const T& value, Args&&... args) {
                std::stringstream ss;
                for (size_t i = 0; i < numTabs; i++) {
                    ss << "\t";
                }
                ss << value;
                echo(ss.str());
                echoIndented(numTabs, std::forward<Args>(args)...);
            }

            template<typename T>
            static void trace(const std::string& name, const T& value) {
                std::stringstream ss;
                ss << name << " = " << value;
                echo(ss.str());
            }

            static void trace(const std::string& name, const std::string& value) {
                echo(name + " = " + value);
            }

            template<typename T, typename F>
            static void trace(const std::string& name, const T& value, const F& formatter) {
                trace(name, formatter << value);
            }

            template<typename T>
            static void traceIterable(const std::string& name, const T& value) {
                unsigned long long counter = 0;
                for (auto& elem : value) {
                    stream() << name << "[" << std::to_string(counter++) << "] = ";
                    echo(elem);
                }
            }

            static void debug(size_t line, const std::string& filename, const std::string& fn) {
                debugBuffer() = std::make_tuple(line, filename, fn);
                static bool ready = false;
                if (!ready) {
                    for (auto& pair : debugLabels) {
                        std::signal(pair.first, printer);
                    }
                    ready = true;
                }
            }

            static void redirect(StreamType& stream) {
                static auto& streamContainer = StreamContainer::instance();
                streamContainer.redirector = defaultRedirector;
                streamContainer.stream = stream;
            }

            static void redirect(const Redirector& redirector) {
                StreamContainer::instance().redirector = redirector;
            }
        };
    }

    template<typename... Args>
    inline void echo(Args&&... args) {
        __detail::DebugContainer<>::echo(std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void echoIndented(size_t numTabs, Args&&... args) {
        __detail::DebugContainer<>::echoIndented(numTabs, std::forward<Args>(args)...);
    }

    template<typename... Args>
    inline void trace(Args&&... args) {
        __detail::DebugContainer<>::trace(std::forward<Args>(args)...);
    }

    template<typename T>
    inline void traceIterable(const std::string& name, const T& value) {
        __detail::DebugContainer<>::traceIterable(name, value);
    }

    inline void debug(size_t line, const std::string& filename, const std::string& fn) {
        __detail::DebugContainer<>::debug(line, filename, fn);
    }

    template<typename T>
    inline void redirect(T& stream) {
        __detail::DebugContainer<>::redirect(stream);
    }

    template<typename T>
    inline void redirect(const T& stream) {
        __detail::DebugContainer<>::redirect(stream);
    }
}

#define FIRST_NAME(v, ...) (#v)

#define ECHO(...) dbg::echo(__VA_ARGS__)
#define ECHOI(numTabs,...) dbg::echoIndented((numTabs), __VA_ARGS__)
#define TRACE(...) dbg::trace(FIRST_NAME(__VA_ARGS__), __VA_ARGS__)
#define TRACE_L(x,...) dbg::trace((x), __VA_ARGS__)
#define TRACE_IT(x) dbg::traceIterable((#x), (x))
#define TRACE_ITL(x) dbg::traceIterable((l), (x))
#define BLANK ECHO("");
#define DEBUG dbg::debug(__LINE__, __FILE__, FUNCTION_NAME);
#define DEBUG_REDIRECT(stream) dbg::redirect(stream)

#if DEBUG_ENABLED == 1
    #define DEBUG_EXEC(...) __VA_ARGS__
#else
    #define DEBUG_EXEC(...) ;
#endif

#endif
