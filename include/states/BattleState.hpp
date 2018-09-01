#ifndef BATTLE_STATE_HPP
#define BATTLE_STATE_HPP

#include <deque>
#include "../battle/EventManager.hpp"
#include "../battle/ScriptVariables.hpp"
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
    explicit BattleState(CoreStructures& gameData);

 private:
    CoreStructures& gameData;
    ScriptVariables scriptVariables;
    EventManager eventManager;
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
    void sortUsedMoves(std::deque<UsedMove>& usedMoves);
    void updateActiveMoveList();
    void processUsedMoves();
    void processMove(UsedMove);
    void showUsedMoveText(const UsedMove&);
    void deductPPIfApplicable(const UsedMove&);
    void processMoveEffects(const UsedMove&);
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
