#ifndef POKEMON_HPP
#define POKEMON_HPP

#include <array>
#include <string>
#include <vector>

enum class Gender {
    Male,
    Female,
    Genderless
};

enum class StatusCondition {
    Normal,
    Sleep,
    Poison,
    Freeze,
    Paralysis,
    Toxic
};

struct Pokemon {
    std::string species;
    std::string nature;
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
    std::string nickname;
    int metAtDate;
    std::string metAtLocation;
    int metAtLevel;
    bool pokerus;
    std::string pokeball;

    // Battle stats
    StatusCondition status;
    int asleepRounds;
    int level;
    int currentHP;
    std::array<int, 6> stats;
};

#endif
