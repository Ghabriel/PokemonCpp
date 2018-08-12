#include "init/register-states.hpp"

#include <memory>
#include "engine/state-system/include.hpp"
#include "states/BattleState.hpp"
#include "states/MenuState.hpp"
#include "states/OverworldState.hpp"
#include "states/TODOState.hpp"

void registerStates(CoreStructures& gameData) {
    gameData.stateMachine->registerState(
        "todo-state",
        std::make_unique<TODOState>()
    );

    gameData.stateMachine->registerState(
        "battle-state",
        std::make_unique<BattleState>(gameData)
    );

    gameData.stateMachine->registerState(
        "menu-state",
        std::make_unique<MenuState>(gameData)
    );

    gameData.stateMachine->registerState(
        "overworld-state",
        std::make_unique<OverworldState>(gameData)
    );
}
