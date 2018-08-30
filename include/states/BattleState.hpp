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
    int chooseMoveAI(const Pokemon&);
    void updateAIVariables();
    void sortUsedMoves(std::deque<UsedMove>& usedMoves);
    void triggerEvent(const std::string& eventName);
    void callMoveEvent(const UsedMove&, const std::string& eventName);
    void updateActiveMoveList();
    void processUsedMoves();
    void processMove(UsedMove);
    void showUsedMoveText(const UsedMove&);
    void deductPPIfApplicable(const UsedMove&);
    void processMoveEffects(const UsedMove&);
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
