#include "states/MenuState.hpp"

#include "engine/entity-system/include.hpp"
#include "engine/input-system/include.hpp"
#include "engine/utils/timing/print-fps.hpp"
#include "CoreStructures.hpp"
#include "core-functions.hpp"

using engine::entitysystem::ComponentManager;
using engine::entitysystem::Entity;
using engine::inputsystem::InputContext;
using engine::utils::Menu;

MenuState::MenuState(CoreStructures& gameData)
 : gameData(gameData),
   menu(*gameData.stateMachine),
   menuEntity(gameData.componentManager->createEntity()) {
    menu.addOption("New Game", "overworld-state");
    menu.addOption("Load Game", "overworld-state");
    menu.addOption("About", "todo-state");

    registerInputContext();
}

void MenuState::executeImpl() {
    engine::utils::printFPS<1>("Menu Update Rate", 50);
}

void MenuState::onEnterImpl() {
    addComponent(menuEntity, menu, gameData);
    enableInputContext("menu-state", gameData);
}

void MenuState::onExitImpl() {
    removeComponent<Menu>(menuEntity, gameData);
    disableInputContext("menu-state", gameData);
}

void MenuState::registerInputContext() {
    InputContext context;

    static const auto hasMenu = [&] {
        return hasComponent<Menu>(menuEntity, gameData);
    };

    static const auto getMenu = [&]() -> Menu& {
        return data<Menu>(menuEntity, gameData);
    };

    static const auto selectOption = [&] {
        if (hasMenu()) {
            sound("fx-select-option", gameData).play();
            getMenu().select();
        }
    };

    context.actions = {
        {"Action", selectOption},
        {"Start", selectOption},
        {"Up", [&] {
            if (hasMenu()) {
                Menu& menu = getMenu();
                size_t focusedOption = (menu.size() + menu.getFocusedOption() - 1) % menu.size();
                menu.focus(focusedOption);
            }
        }},
        {"Down", [&] {
            if (hasMenu()) {
                Menu& menu = getMenu();
                size_t focusedOption = (menu.getFocusedOption() + 1) % menu.size();
                menu.focus(focusedOption);
            }
        }}
    };

    gameData.inputDispatcher->registerContext("menu-state", context);
}
