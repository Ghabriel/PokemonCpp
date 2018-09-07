#ifndef BATTLE_CONTROLLER_HPP
#define BATTLE_CONTROLLER_HPP

#include <deque>
#include <vector>
#include "../engine/entity-system/types.hpp"
#include "EventManager.hpp"
#include "ScriptVariables.hpp"

struct BoundMove;
struct CoreStructures;
struct Pokemon;
class TextProvider;

class BattleController {
    using Entity = engine::entitysystem::Entity;
 public:
    enum class State {
        PENDING_START,
        READY,
        VICTORY,
        DEFEAT,
        DRAW,
        // PENDING_SWITCH,
    };

    BattleController() = default;
    BattleController(Entity battleEntity, TextProvider&, CoreStructures& gameData);

    void startBattle();
    void abort();
    void tick();
    bool hasPendingEvents() const;
    State getState() const;

    int chooseMoveAI(const Pokemon&);
    void processTurn(const std::vector<BoundMove>& usedMoves);

 private:
    Entity battleEntity;
    TextProvider* textProvider;
    CoreStructures* gameData;
    Battle* battle;
    ScriptVariables scriptVariables;
    EventManager eventManager;
    State state;

    void sortUsedMoves(std::vector<BoundMove>& usedMoves);
    void updateActiveFlags();
    void processUsedMoves();
    void processMove(BoundMove);
    void prepareScriptsForMove(const BoundMove&);
    void showUsedMoveText(const BoundMove&);
    void deductPPIfApplicable(const BoundMove&);
    void processMoveEffects(const BoundMove&);
    void checkFaintedPokemon();

    void showText(const std::string& content);

    bool isPlayerUnableToContinue() const;
    bool isOpponentUnableToContinue() const;
    bool isEveryPokemonFainted(const std::vector<Entity>& team) const;
    bool isOpponent(Entity) const;
    Pokemon& pokemon(Entity);
    void loadDetailedPokemonData();
    void loadMoves(Entity);
    void loadSpeciesData(Entity);
};

#endif
