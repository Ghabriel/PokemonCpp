#ifndef SETTINGS_HPP
#define SETTINGS_HPP

#include <string>
#include "engine/resource-system/json/include.hpp"

class Settings {
 public:
    Settings();
    std::string getInitialState() const;
    std::string getWindowTitle() const;
    int getInitialWindowWidth() const;
    int getInitialWindowHeight() const;
    float getPlayerWalkingSpeed() const;
    int getTileSize() const;

 private:
    JsonValue data;
};

#endif
