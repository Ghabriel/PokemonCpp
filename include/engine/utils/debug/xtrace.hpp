/* created by Ghabriel Nunes <ghabriel.nunes@gmail.com> [2017] */
#ifndef UTILS_XTRACE_HPP
#define UTILS_XTRACE_HPP

#include <functional>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <typeinfo>
#include <type_traits>
#include <unordered_map>
#include "debug.hpp"

#if ALLOW_DEBUG_USAGE == 1

namespace dbg {
    namespace __detail {
        template<int P> struct priority : public priority<P-1> {};
        template<> struct priority<0> {};

        template<typename T>
        struct it_underlying_type {
         private:
            template<typename C,
                typename Begin = decltype(*std::declval<C>().begin()),
                typename End = decltype(*std::declval<C>().end()),
                typename =
                    typename std::enable_if<std::is_same<Begin, End>::value>::type>
            static Begin test(const C&);
            static void test(...);

         public:
            using type = decltype(it_underlying_type::test(T{}));
        };

        template<typename T1, typename T2>
        struct is_same_decay {
            static constexpr bool value = std::is_same<
                typename std::decay<T1>::type,
                typename std::decay<T2>::type
            >::value;
        };

        template<typename Container>
        struct is_linear_container {
         private:
            template<template<typename...> typename C, typename T, typename... Ts,
                typename = typename std::enable_if<std::is_void<T>::value>::type>
            static std::nullptr_t* first_template(const C<T, Ts...>&, priority<1>);

            template<template<typename...> typename C, typename T, typename... Ts>
            static T* first_template(const C<T, Ts...>&, priority<0>);

            template<template<typename...> typename C, typename T, typename... Ts>
            static decltype(first_template(C<T, Ts...>{}, priority<1>{}))
                first_template(const C<T, Ts...>&);

         public:
            static constexpr bool value = is_same_decay<
                typename it_underlying_type<Container>::type,
                decltype(*is_linear_container::first_template(Container{}))
            >::value;
        };
    }

    class XTraceFormatter {
        template<int P>
        using priority = __detail::priority<P>;

        template<typename Container>
        using is_linear_container = __detail::is_linear_container<Container>;
     public:
        static XTraceFormatter instance() {
            static XTraceFormatter inst;
            return inst;
        }

        std::string operator<<(bool) const;
        std::string operator<<(char) const;
        std::string operator<<(const char*) const;
        std::string operator<<(const std::string&) const;
        std::string operator<<(void*) const;
        template<typename T>
        std::string operator<<(T* const) const;
        template<typename T>
        std::string operator<<(const std::unique_ptr<T>&) const;
        template<typename T>
        std::string operator<<(const std::shared_ptr<T>&) const;
        template<typename T>
        std::string operator<<(const std::weak_ptr<T>&) const;
        template<typename T>
        std::string operator<<(const std::reference_wrapper<T>&) const;
        template<typename T>
        std::string operator<<(const T&) const;
        template<typename T1, typename T2>
        std::string operator<<(const std::pair<T1, T2>& pair) const;
        template<typename... Types>
        std::string operator<<(const std::tuple<Types...>& tuple) const;
        template<typename T, size_t N>
        std::string operator<<(const std::array<T,N>& c) const {
            return formatContainer(c);
        }
        std::string operator<<(const std::vector<bool>& c) const {
            return formatContainer(c);
        }
        template<typename K, typename V>
        std::string operator<<(const std::map<K,V>& c) const {
            return formatContainer(c);
        }
        template<typename K, typename V>
        std::string operator<<(const std::unordered_map<K,V>& c) const {
            return formatContainer(c);
        }

     private:
        XTraceFormatter() = default;

        template<typename T,
            typename = typename std::enable_if<std::is_arithmetic<T>::value>::type>
        std::string operator()(const T& value, priority<2>) const {
            return std::to_string(value);
        }

        template<template<typename...> typename C, typename... Ts,
            typename = typename std::enable_if<is_linear_container<C<Ts...>>::value>::type>
        std::string operator()(const C<Ts...>& value, priority<1>) const {
            return formatContainer(value);
        }

        template<typename T>
        std::string operator()(const T& value, priority<0>) const {
            std::stringstream ss;
            ss << "UNPRINTABLE (typeid: " << typeid(value).name() << ")";
            return ss.str();
        }

        template<typename T>
        std::string smartPointer(T* const value) const {
            std::stringstream ss;
            ss << "(" << static_cast<const void*>(value)
               << ") -> " << ((*this) << (*value));
            return ss.str();
        }

        template<typename T>
        std::string formatContainer(const T&) const;

        template<typename T>
        std::string construct(const T&) const;

        template<typename T, typename... Args>
        std::string construct(const T&, Args&&...) const;

        template<typename... Types, size_t... I>
        std::string extract(const std::tuple<Types...>&, std::index_sequence<I...>) const;
    };


    inline std::string XTraceFormatter::operator<<(void* value) const {
        std::stringstream ss;
        ss << value;
        return ss.str();
    }

    template<typename T>
    inline std::string XTraceFormatter::operator<<(T* const value) const {
        std::stringstream ss;
        ss << static_cast<const void*>(value) << " -> " << ((*this) << (*value));
        return ss.str();
    }

    template<typename T>
    inline std::string XTraceFormatter::operator<<(const std::unique_ptr<T>& value) const {
        return "unique" + smartPointer(value.get());
    }

    template<typename T>
    inline std::string XTraceFormatter::operator<<(const std::shared_ptr<T>& value) const {
        return "shared" + smartPointer(value.get());
    }

    template<typename T>
    inline std::string XTraceFormatter::operator<<(const std::weak_ptr<T>& value) const {
        if (auto shared = value.lock()) {
            return "weak" + smartPointer(shared.get());
        }

        return "weak(invalid)";
    }

    template<typename T>
    std::string XTraceFormatter::operator<<(const std::reference_wrapper<T>& value) const {
        return (*this) << value.get();
    }

    inline std::string XTraceFormatter::operator<<(bool value) const {
        return value ? "true" : "false";
    }

    inline std::string XTraceFormatter::operator<<(char value) const {
        return "'" + std::string(1, value) + "'";
    }

    inline std::string XTraceFormatter::operator<<(const char* value) const {
        std::stringstream ss;
        ss << '"' << value << '"';
        return ss.str();
    }

    inline std::string XTraceFormatter::operator<<(const std::string& value) const {
        return (*this) << value.c_str();
    }

    template<typename T>
    inline std::string XTraceFormatter::operator<<(const T& value) const {
        return (*this)(value, priority<2>{});
    }

    template<typename T1, typename T2>
    inline std::string XTraceFormatter::operator<<(const std::pair<T1, T2>& pair) const {
        return '{' + ((*this) << pair.first) + ", " + ((*this) << pair.second) + '}';
    }

    template<typename... Types>
    inline std::string XTraceFormatter::operator<<(const std::tuple<Types...>& tuple) const {
        return '{' +
                extract(tuple, std::make_index_sequence<sizeof...(Types)>())
                + '}';
    }

    template<typename T>
    inline std::string XTraceFormatter::formatContainer(const T& container) const {
        std::stringstream ss;
        ss << '{';
        bool first = true;
        for (const auto& value : container) {
            if (!first) {
                ss << ", ";
            }
            ss << ((*this) << value);
            first = false;
        }
        ss << '}';
        return ss.str();
    }

    template<typename T>
    inline std::string XTraceFormatter::construct(const T& value) const {
        return ((*this) << value);
    }

    template<typename T, typename... Args>
    inline std::string XTraceFormatter::construct(const T& value, Args&&... args) const {
        return ((*this) << value) + ", " + construct(std::forward<Args>(args)...);
    }

    template<typename... Types, size_t... I>
    inline std::string XTraceFormatter::extract(const std::tuple<Types...>& tuple,
        std::index_sequence<I...>) const {

        return construct(std::get<I>(tuple)...);
    }
}

namespace __detail {
    const auto xTraceFormatter = dbg::XTraceFormatter::instance();
}

#define XTRACE(x) dbg::trace((#x), (x), ::__detail::xTraceFormatter)

#else

#define XTRACE(...) {int debug_usage;}

#endif

#endif
