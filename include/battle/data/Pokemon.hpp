#ifndef POKEMON_HPP
#define POKEMON_HPP

#include <array>
#include <string>
#include <vector>

enum class Nature {
    Hardy,
    Lonely,
    Adamant,
    Naughty,
    Brave,
    Bold,
    Docile,
    Impish,
    Lax,
    Relaxed,
    Modest,
    Mild,
    Bashful,
    Rash,
    Quiet,
    Calm,
    Gentle,
    Careful,
    Quirky,
    Sassy,
    Timid,
    Hasty,
    Jolly,
    Naive,
    Serious,
    Count // for generation only
};

enum class Gender {
    Male,
    Female,
    Genderless
};

enum class StatusCondition {
    Normal,
    Burn,
    Freeze,
    Paralysis,
    Poison,
    Toxic,
    Sleep
};

struct Pokemon {
    std::string species;
    Nature nature;
    std::string heldItem;
    int experiencePoints;
    std::string ability;
    std::array<int, 6> ev;
    std::array<int, 6> iv;
    std::vector<std::string> moves;
    std::vector<int> pp;
    std::vector<int> ppUps;
    int eggStepsToHatch; // < 0 if not an egg
    Gender gender;
    int form;
    bool isNicknamed;
    std::string displayName;
    int metAtDate;
    std::string metAtLocation;
    int metAtLevel;
    bool pokerus;
    std::string pokeball;

    // Battle stats
    StatusCondition status;
    int asleepRounds;
    int level;
    std::array<int, 6> stats;
    float currentHP;
};

#endif
