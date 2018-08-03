#include "init/register-states.hpp"

#include <memory>
#include "engine/state-system/include.hpp"
#include "states/MenuState.hpp"
#include "states/TODOState.hpp"

void registerStates(CoreStructures& gameData) {
    gameData.stateMachine->registerState(
        "todo-state",
        std::make_unique<TODOState>()
    );

    gameData.stateMachine->registerState(
        "menu-state",
        std::make_unique<MenuState>(gameData)
    );
}
