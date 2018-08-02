#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>
#include "engine/resource-system/json/include.hpp"

class Settings {
 public:
    Settings();
    std::string getInitialState() const;

 private:
    JsonValue data;
};

#endif
