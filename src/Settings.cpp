#include "Settings.hpp"

#include <fstream>
#include "ResourceFiles.hpp"

Settings::Settings() {
    std::ifstream settingsFile(ResourceFiles::SETTINGS);
    data = parseJSON(settingsFile);
}

std::string Settings::getInitialState() const {
    return data["initial-state"].get<std::string>();
}
