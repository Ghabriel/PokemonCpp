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
    int getMinWindowWidth() const;
    int getMinWindowHeight() const;
    float getPlayerWalkingSpeed() const;
    int getTileSize() const;
    std::string getPokemonBackSpritesFolder() const;
    std::string getPokemonFrontSpritesFolder() const;

 private:
    JsonValue data;
};

#endif
