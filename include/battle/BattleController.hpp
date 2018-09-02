#ifndef BATTLE_CONTROLLER_HPP
#define BATTLE_CONTROLLER_HPP

#include <deque>
#include "../engine/entity-system/types.hpp"
#include "EventManager.hpp"
#include "ScriptVariables.hpp"

struct CoreStructures;
struct Pokemon;
struct UsedMove;

class BattleController {
    using Entity = engine::entitysystem::Entity;
 public:
    enum class State {
        PENDING_START,
        READY,
        VICTORY,
        DEFEAT,
        // PENDING_SWITCH,
    };

    BattleController() = default;
    BattleController(Entity battleEntity, CoreStructures& gameData);

    void startBattle();
    void abort();
    void tick();
    bool hasPendingEvents() const;
    State getState() const;

    int chooseMoveAI(const Pokemon&);
    void processTurn(const std::deque<UsedMove>& usedMoves);

 private:
    Entity battleEntity;
    CoreStructures* gameData;
    Battle* battle;
    ScriptVariables scriptVariables;
    EventManager eventManager;
    State state;

    void sortUsedMoves(std::deque<UsedMove>& usedMoves);
    void updateActiveMoveList();
    void processUsedMoves();
    void processMove(UsedMove);
    void showUsedMoveText(const UsedMove&);
    void deductPPIfApplicable(const UsedMove&);
    void processMoveEffects(const UsedMove&);
    void checkFaintedPokemon();

    void showText(const std::string& content);
    void showMoveText(const std::string& content);

    Pokemon& pokemon(Entity entity);
    void loadDetailedPokemonData();
    void loadMoves(Entity);
    void loadSpeciesData(Entity);
};

#endif
