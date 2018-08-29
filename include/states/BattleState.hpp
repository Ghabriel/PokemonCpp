#ifndef BATTLE_STATE_HPP
#define BATTLE_STATE_HPP

#include <deque>
#include "../engine/entity-system/types.hpp"
#include "../engine/state-system/include.hpp"

struct Battle;
struct CoreStructures;
struct Move;
struct Pokemon;
struct PokemonSpeciesData;
struct UsedMove;

class BattleState : public engine::statesystem::State {
    using Entity = engine::entitysystem::Entity;
 public:
    BattleState(CoreStructures& gameData);

 private:
    CoreStructures& gameData;
    Entity battleEntity;
    Battle* battle;
    int selectedAction;

    void onEnterImpl() override;
    void onExitImpl() override;
    void executeImpl() override;

    void actionSelectionScreen();
    void moveSelectionScreen();
    void processTurn();
    UsedMove getUsedMoveBy(Entity user, Entity target, int selectedAction);
    void updateAIVariables();
    size_t chooseMoveAI(const Pokemon&);
    void callMoveEvent(const UsedMove&, const std::string& eventName);
    void sortUsedMoves(std::deque<UsedMove>& usedMoves);
    void processUsedMoves();
    void processNextMove(UsedMove);
    void processPlayerMove(int moveIndex);
    void processOpponentMove(int moveIndex);
    void processMove(Entity user, Entity target, Move& move);
    void updateMoveVariables(Entity user, Entity target);
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
