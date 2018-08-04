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

float Settings::getPlayerWalkingSpeed() const {
    return std::stof(data["player-walking-speed"].asString());
}

int Settings::getTileSize() const {
    return data["tile-size"].asInt();
}
