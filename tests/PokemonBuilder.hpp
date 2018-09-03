#ifndef POKEMON_BUILDER_HPP
#define POKEMON_BUILDER_HPP

#include <optional>
#include "battle/data/Pokemon.hpp"
#include "battle/data/PokemonSpeciesData.hpp"
#include "battle/helpers/generate-pokemon.hpp"
#include "engine/resource-system/include.hpp"

class PokemonBuilder {
    using ResourceStorage = engine::resourcesystem::ResourceStorage;
 public:
    PokemonBuilder(
        ResourceStorage&,
        const std::string& speciesId,
        size_t level
    );
    PokemonBuilder& withNature(Nature);
    PokemonBuilder& withHeldItem(const std::string& itemId);
    PokemonBuilder& withExp(int expPoints);
    PokemonBuilder& withAbility(const std::string& abilityId);
    PokemonBuilder& withEVs(const std::array<int, 6>&);
    PokemonBuilder& withIVs(const std::array<int, 6>&);
    PokemonBuilder& withMoves(const std::vector<std::string>& moveIds);
    PokemonBuilder& withPPs(const std::vector<int>& pps);
    PokemonBuilder& withPPUps(const std::vector<int>& ppUps);
    PokemonBuilder& withGender(Gender);
    PokemonBuilder& withForm(int form);
    PokemonBuilder& withNickname(const std::string&);
    PokemonBuilder& withPokerus();
    PokemonBuilder& withPokeball(const std::string& pokeballId);
    PokemonBuilder& withStatusCondition(StatusCondition);
    PokemonBuilder& withAsleepCounter(int asleepRounds);
    PokemonBuilder& withCurrentHP(int currentHP);
    Pokemon build() const;

 private:
    ResourceStorage& storage;
    std::string species;
    size_t level;
    std::optional<Nature> nature;
    std::optional<std::string> heldItem;
    std::optional<int> experiencePoints;
    std::optional<std::string> ability;
    std::optional<std::array<int, 6>> ev;
    std::optional<std::array<int, 6>> iv;
    std::optional<std::vector<std::string>> moves;
    std::optional<std::vector<int>> pp;
    std::optional<std::vector<int>> ppUps;
    std::optional<Gender> gender;
    std::optional<int> form;
    std::optional<std::string> displayName;
    std::optional<bool> pokerus;
    std::optional<std::string> pokeball;
    std::optional<StatusCondition> status;
    std::optional<int> asleepRounds;
    std::optional<float> currentHP;
};

PokemonBuilder::PokemonBuilder(
    ResourceStorage& storage,
    const std::string& speciesId,
    size_t level
) : storage(storage),
    species(speciesId),
    level(level) { }

PokemonBuilder& PokemonBuilder::withNature(Nature _nature) {
    nature = _nature;
    return *this;
}

PokemonBuilder& PokemonBuilder::withHeldItem(const std::string& itemId) {
    heldItem = itemId;
    return *this;
}

PokemonBuilder& PokemonBuilder::withExp(int expPoints) {
    experiencePoints = expPoints;
    return *this;
}

PokemonBuilder& PokemonBuilder::withAbility(const std::string& abilityId) {
    ability = abilityId;
    return *this;
}

PokemonBuilder& PokemonBuilder::withEVs(const std::array<int, 6>& evs) {
    ev = evs;
    return *this;
}

PokemonBuilder& PokemonBuilder::withIVs(const std::array<int, 6>& ivs) {
    iv = ivs;
    return *this;
}

PokemonBuilder& PokemonBuilder::withMoves(const std::vector<std::string>& moveIds) {
    moves = moveIds;
    return *this;
}

PokemonBuilder& PokemonBuilder::withPPs(const std::vector<int>& pps) {
    pp = pps;
    return *this;
}

PokemonBuilder& PokemonBuilder::withPPUps(const std::vector<int>& _ppUps) {
    ppUps = _ppUps;
    return *this;
}

PokemonBuilder& PokemonBuilder::withGender(Gender _gender) {
    gender = _gender;
    return *this;
}

PokemonBuilder& PokemonBuilder::withForm(int _form) {
    form = _form;
    return *this;
}

PokemonBuilder& PokemonBuilder::withNickname(const std::string& nickname) {
    displayName = nickname;
    return *this;
}

PokemonBuilder& PokemonBuilder::withPokerus() {
    pokerus = true;
    return *this;
}

PokemonBuilder& PokemonBuilder::withPokeball(const std::string& pokeballId) {
    pokeball = pokeballId;
    return *this;
}

PokemonBuilder& PokemonBuilder::withStatusCondition(StatusCondition statusCondition) {
    status = statusCondition;
    return *this;
}

PokemonBuilder& PokemonBuilder::withAsleepCounter(int _asleepRounds) {
    asleepRounds = _asleepRounds;
    return *this;
}

PokemonBuilder& PokemonBuilder::withCurrentHP(int _currentHP) {
    currentHP = _currentHP;
    return *this;
}

#include "engine/utils/debug/xtrace.hpp"

Pokemon PokemonBuilder::build() const {
    using namespace generation;
    PokemonSpeciesData& speciesData = storage.get<PokemonSpeciesData>("pokemon-" + species);
    Pokemon pokemon;

    pokemon.species = species;
    pokemon.nature = nature ? nature.value() : pickNature();
    pokemon.heldItem = heldItem.value_or("");
    pokemon.experiencePoints = experiencePoints.value_or(0);
    pokemon.ability = ability.value_or(""); // TODO
    pokemon.ev = ev ? ev.value() : std::array{0, 0, 0, 0, 0, 0};
    pokemon.iv = iv ? iv.value() : pickIVs();
    pokemon.moves = moves ? moves.value() : pickMoves(speciesData, level);
    pokemon.pp = pp ? pp.value() : pickPPs(storage, pokemon.moves);
    pokemon.ppUps = ppUps ? ppUps.value() : std::vector<int>{0, 0, 0, 0};
    pokemon.eggStepsToHatch = -1;
    pokemon.gender = gender ? gender.value() : pickGender(speciesData);
    pokemon.form = form.value_or(0);
    pokemon.isNicknamed = displayName.has_value();
    pokemon.displayName = displayName.value_or(speciesData.displayName);
    pokemon.metAtDate = 0;
    pokemon.metAtLocation = "testing-area";
    pokemon.metAtLevel = level;
    pokemon.pokerus = pokerus.value_or(false);
    pokemon.pokeball = pokeball.value_or("");
    pokemon.status = status.value_or(StatusCondition::Normal);
    pokemon.asleepRounds = asleepRounds.value_or(0);
    pokemon.level = level;
    pokemon.stats = {0, 0, 0, 0, 0, 0};
    pickStats(speciesData, pokemon);
    pokemon.currentHP = currentHP.value_or(pokemon.stats[0]);

    return pokemon;
}


#endif
