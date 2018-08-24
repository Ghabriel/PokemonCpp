#ifndef BATTLE_UTILS_HPP
#define BATTLE_UTILS_HPP

struct CoreStructures;
struct Move;
struct Pokemon;
struct PokemonSpeciesData;

int getAttackStatForMove(const Pokemon& pokemon, const Move& move);
int getDefenseStatForMove(const Pokemon& pokemon, const Move& move);
PokemonSpeciesData& getSpecies(const Pokemon& pokemon, CoreStructures& gameData);
float getTypeEffectiveness(const PokemonSpeciesData& species, const Move& move);
int calculateExpGain(
    const Pokemon& winner,
    const Pokemon& fainted,
    const PokemonSpeciesData& faintedSpecies
);

#endif
