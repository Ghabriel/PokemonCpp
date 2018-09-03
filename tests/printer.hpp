#include <sstream>
#include <type_traits>
#include "battle/data/Pokemon.hpp"
#include "engine/utils/debug/xtrace.hpp"

std::string operator<<(dbg::XTraceFormatter x, const Nature& enumValue) {
    static const std::unordered_map<Nature, std::string> labels = {
        {Nature::Hardy, "Hardy"},
        {Nature::Lonely, "Lonely"},
        {Nature::Adamant, "Adamant"},
        {Nature::Naughty, "Naughty"},
        {Nature::Brave, "Brave"},
        {Nature::Bold, "Bold"},
        {Nature::Docile, "Docile"},
        {Nature::Impish, "Impish"},
        {Nature::Lax, "Lax"},
        {Nature::Relaxed, "Relaxed"},
        {Nature::Modest, "Modest"},
        {Nature::Mild, "Mild"},
        {Nature::Bashful, "Bashful"},
        {Nature::Rash, "Rash"},
        {Nature::Quiet, "Quiet"},
        {Nature::Calm, "Calm"},
        {Nature::Gentle, "Gentle"},
        {Nature::Careful, "Careful"},
        {Nature::Quirky, "Quirky"},
        {Nature::Sassy, "Sassy"},
        {Nature::Timid, "Timid"},
        {Nature::Hasty, "Hasty"},
        {Nature::Jolly, "Jolly"},
        {Nature::Naive, "Naive"},
        {Nature::Serious, "Serious"},
    };

    return labels.at(enumValue);
}

std::string operator<<(dbg::XTraceFormatter x, const Gender& enumValue) {
    static const std::unordered_map<Gender, std::string> labels = {
        {Gender::Male, "Male"},
        {Gender::Female, "Female"},
        {Gender::Genderless, "Genderless"}
    };

    return labels.at(enumValue);
}

std::string operator<<(dbg::XTraceFormatter x, const StatusCondition& enumValue) {
    static const std::unordered_map<StatusCondition, std::string> labels = {
        {StatusCondition::Normal, "Normal"},
        {StatusCondition::Burn, "Burn"},
        {StatusCondition::Freeze, "Freeze"},
        {StatusCondition::Paralysis, "Paralysis"},
        {StatusCondition::Poison, "Poison"},
        {StatusCondition::Toxic, "Toxic"},
        {StatusCondition::Sleep, "Sleep"}
    };

    return labels.at(enumValue);
}

std::string operator<<(dbg::XTraceFormatter x, const Pokemon& pokemon) {
    std::stringstream ss;
    ss << "Pokemon {\n";
    ss << "\tspecies: " << (x << pokemon.species) << "\n";
    ss << "\tnature: " << (x << pokemon.nature) << "\n";
    ss << "\theldItem: " << (x << pokemon.heldItem) << "\n";
    ss << "\texperiencePoints: " << (x << pokemon.experiencePoints) << "\n";
    ss << "\tability: " << (x << pokemon.ability) << "\n";
    ss << "\tev: " << (x << pokemon.ev) << "\n";
    ss << "\tiv: " << (x << pokemon.iv) << "\n";
    ss << "\tmoves: " << (x << pokemon.moves) << "\n";
    ss << "\tpp: " << (x << pokemon.pp) << "\n";
    ss << "\tppUps: " << (x << pokemon.ppUps) << "\n";
    ss << "\teggStepsToHatch: " << (x << pokemon.eggStepsToHatch) << "\n";
    ss << "\tgender: " << (x << pokemon.gender) << "\n";
    ss << "\tform: " << (x << pokemon.form) << "\n";
    ss << "\tisNicknamed: " << (x << pokemon.isNicknamed) << "\n";
    ss << "\tdisplayName: " << (x << pokemon.displayName) << "\n";
    ss << "\tmetAtDate: " << (x << pokemon.metAtDate) << "\n";
    ss << "\tmetAtLocation: " << (x << pokemon.metAtLocation) << "\n";
    ss << "\tmetAtLevel: " << (x << pokemon.metAtLevel) << "\n";
    ss << "\tpokerus: " << (x << pokemon.pokerus) << "\n";
    ss << "\tpokeball: " << (x << pokemon.pokeball) << "\n";

    ss << "\tstatus: " << (x << pokemon.status) << "\n";
    ss << "\tasleepRounds: " << (x << pokemon.asleepRounds) << "\n";
    ss << "\tlevel: " << (x << pokemon.level) << "\n";
    ss << "\tstats: " << (x << pokemon.stats) << "\n";
    ss << "\tcurrentHP: " << (x << pokemon.currentHP) << "\n";
    ss << "}";
    return ss.str();
}