#ifndef BATTLE_STATE_HPP
#define BATTLE_STATE_HPP

#include "../engine/entity-system/types.hpp"
#include "../engine/state-system/include.hpp"

struct Battle;
struct CoreStructures;
struct Move;
struct Pokemon;
struct PokemonSpeciesData;

class BattleState : public engine::statesystem::State {
    using Entity = engine::entitysystem::Entity;
 public:
    BattleState(CoreStructures& gameData);

 private:
    CoreStructures& gameData;
    Entity battleEntity;
    Battle* battle;
    int selectedAction;

    void registerInputContext();
    void onEnterImpl() override;
    void onExitImpl() override;
    void executeImpl() override;

    void actionSelectionScreen();
    void moveSelectionScreen();
    void processTurn();
    void updateAIVariables();
    size_t chooseMoveAI(const Pokemon&);
    void processPlayerMove(size_t moveIndex);
    void processOpponentMove(size_t moveIndex);
    void processMove(Entity user, Entity target, Move& move);
    void checkFaintedPokemon();
    void blackOutScreen();
    void rewardScreen();

    void showText(const std::string&);
    void showMoveText(const std::string&);

    Pokemon& pokemon(Entity);
    std::vector<Move*> moves(Entity);
    PokemonSpeciesData& species(Entity);
    void loadDetailedPokemonData();
    void loadMoves(Entity);
    void loadSpeciesData(Entity);
};

#endif
