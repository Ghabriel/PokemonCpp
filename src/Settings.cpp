#include "Settings.hpp"

#include <fstream>
#include "ResourceFiles.hpp"

Settings::Settings() {
    std::ifstream settingsFile(ResourceFiles::SETTINGS);
    data = parseJSON(settingsFile);
}

std::string Settings::getInitialState() const {
    return data["initial-state"].asString();
}

std::string Settings::getWindowTitle() const {
    return data["window-title"].asString();
}

int Settings::getInitialWindowWidth() const {
    return data["initial-window-width"].asInt();
}

int Settings::getInitialWindowHeight() const {
    return data["initial-window-height"].asInt();
}

int Settings::getMinWindowWidth() const {
    return data["min-window-width"].asInt();
}

int Settings::getMinWindowHeight() const {
    return data["min-window-height"].asInt();
}

float Settings::getPlayerWalkingSpeed() const {
    return std::stof(data["player-walking-speed"].asString());
}

int Settings::getTileSize() const {
    return data["tile-size"].asInt();
}

std::string Settings::getPokemonBackSpritesFolder() const {
    return data["pokemon-back-sprites"].asString();
}

std::string Settings::getPokemonFrontSpritesFolder() const {
    return data["pokemon-front-sprites"].asString();
}
