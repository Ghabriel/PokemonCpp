#ifndef PUBLIC_POKEMON_DATA_HPP
#define PUBLIC_POKEMON_DATA_HPP

#include <optional>
#include <string>
#include "../data/Gender.hpp"
#include "../data/StatusCondition.hpp"

struct AllyPokemonData {
    int experiencePoints;
    int currentHP;
    int maxHP;
};

struct PublicPokemonData {
    std::string species;
    Gender gender;
    int form;
    bool isNicknamed;
    std::string displayName;
    StatusCondition statusCondition;
    int level;
    float currentHpPercentage;
    std::optional<AllyPokemonData> allyData;
};

#endif
