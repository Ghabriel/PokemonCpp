#ifndef ENCOUNTER_DATA_HPP
#define ENCOUNTER_DATA_HPP

#include <string>
#include <unordered_map>
#include <vector>

struct EncounterData {
    std::string pokemon;
    int minLevel;
    int maxLevel;
    int rate;
};

struct MapEncounterData {
    using Environment = std::string;
    std::unordered_map<Environment, std::vector<EncounterData>> encounterData;
};

#endif
