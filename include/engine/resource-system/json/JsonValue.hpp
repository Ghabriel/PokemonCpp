#ifndef JSON_VALUE_HPP
#define JSON_VALUE_HPP

#include <iostream>
#include <memory>
#include <typeindex>
#include "Scanner.hpp"

class JsonValue;

namespace __detail {
    using Array = std::vector<JsonValue>;
    using Map = std::unordered_map<std::string, JsonValue>;

    template<typename T>
    class JsonIteratorWrapper {
     public:
        using iterator = typename T::iterator;
        using const_iterator = typename T::const_iterator;

        JsonIteratorWrapper(const JsonValue&);

        iterator begin();
        const_iterator begin() const;
        const_iterator cbegin() const;
        iterator end();
        const_iterator end() const;
        const_iterator cend() const;

     private:
        const JsonValue& value;
    };
}

using JsonArrayIterator = __detail::JsonIteratorWrapper<__detail::Array>;
using JsonMapIterator = __detail::JsonIteratorWrapper<__detail::Map>;

class JsonValue {
    using Array = __detail::Array;
    using Map = __detail::Map;
 public:
    template<typename T>
    JsonValue(const T&);

    template<typename T>
    bool is() const;

    template<typename T>
    T& get() const;

    JsonArrayIterator asIterableArray() const {
        return *this;
    }

    JsonMapIterator asIterableMap() const {
        return *this;
    }

    // Array access
    JsonValue& operator[](int index);
    const JsonValue& operator[](int index) const;
    JsonValue& at(int index);
    const JsonValue& at(int index) const;

    // Map access
    JsonValue& operator[](const std::string& key);
    const JsonValue& operator[](const std::string& key) const;
    JsonValue& at(const std::string& key);
    const JsonValue& at(const std::string& key) const;

 private:
    std::type_index type;
    std::shared_ptr<void> value;
};

template <typename T>
JsonValue::JsonValue(const T& value) : type(typeid(T)), value(new T(value)) { }

template<typename T>
bool JsonValue::is() const {
    return type == typeid(T);
}

template<typename T>
T& JsonValue::get() const {
    return *static_cast<T*>(value.get());
}

JsonValue& JsonValue::operator[](const std::string& key) {
    return get<Map>().at(key);
}

const JsonValue& JsonValue::operator[](const std::string& key) const {
    return get<Map>().at(key);
}

JsonValue& JsonValue::at(const std::string& key) {
    return get<Map>().at(key);
}

const JsonValue& JsonValue::at(const std::string& key) const {
    return get<Map>().at(key);
}

JsonValue& JsonValue::operator[](int index) {
    return get<Array>().at(index);
}

const JsonValue& JsonValue::operator[](int index) const {
    return get<Array>().at(index);
}

JsonValue& JsonValue::at(int index) {
    return get<Array>().at(index);
}

const JsonValue& JsonValue::at(int index) const {
    return get<Array>().at(index);
}

namespace __detail {
    template<typename T>
    JsonIteratorWrapper<T>::JsonIteratorWrapper(const JsonValue& value) : value(value) { }

    template<typename T>
    typename JsonIteratorWrapper<T>::iterator
    JsonIteratorWrapper<T>::begin() { return value.get<T>().begin(); }

    template<typename T>
    typename JsonIteratorWrapper<T>::const_iterator
    JsonIteratorWrapper<T>::begin() const { return value.get<T>().cbegin(); }

    template<typename T>
    typename JsonIteratorWrapper<T>::const_iterator
    JsonIteratorWrapper<T>::cbegin() const { return value.get<T>().begin(); }

    template<typename T>
    typename JsonIteratorWrapper<T>::iterator
    JsonIteratorWrapper<T>::end() { return value.get<T>().end(); }

    template<typename T>
    typename JsonIteratorWrapper<T>::const_iterator
    JsonIteratorWrapper<T>::end() const { return value.get<T>().cend(); }

    template<typename T>
    typename JsonIteratorWrapper<T>::const_iterator
    JsonIteratorWrapper<T>::cend() const { return value.get<T>().end(); }
}

#endif
