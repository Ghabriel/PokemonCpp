#ifndef INTERACTIVE_LAYER_HPP
#define INTERACTIVE_LAYER_HPP

#include <string>
#include <vector>
#include "../engine/entity-system/types.hpp"

struct Battle;
class BattleController;
struct CoreStructures;
struct Move;
struct UsedMove;
struct Pokemon;

class InteractiveLayer {
    using Entity = engine::entitysystem::Entity;
 public:
    InteractiveLayer() = default;
    InteractiveLayer(Entity battleEntity, BattleController&, CoreStructures&);

    void startBattle();
    void abort();
    void tick();

 private:
    Entity battleEntity;
    BattleController* battleController;
    CoreStructures* gameData;
    Battle* battle;
    int selectedAction;

    void actionSelectionScreen();
    void moveSelectionScreen();
    void processTurn();
    UsedMove getUsedMoveBy(Entity user, Entity target, int selectedAction);
    void blackOutScreen();
    void rewardScreen();


    Pokemon& pokemon(Entity);
    std::vector<Move*> moves(Entity);
    void showText(const std::string& content);
};

#endif
