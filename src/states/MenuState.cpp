#include "states/MenuState.hpp"

#include "engine/entity-system/include.hpp"
#include "engine/input-system/include.hpp"
#include "engine/resource-system/ResourceStorage.hpp"
#include "engine/sfml/sound-system/Music.hpp"
#include "engine/utils/timing/print-fps.hpp"
#include "CoreStructures.hpp"

using engine::entitysystem::ComponentManager;
using engine::entitysystem::Entity;
using engine::inputsystem::InputContext;
using engine::utils::Menu;
using engine::soundsystem::Music;
using engine::resourcesystem::ResourceStorage;

MenuState::MenuState(CoreStructures& gameData)
 : gameData(gameData),
   menu(*gameData.stateMachine),
   menuEntity(gameData.componentManager->createEntity()) {
    menu.addOption("New Game", "todo-state");
    menu.addOption("Load Game", "todo-state");
    menu.addOption("About", "todo-state");

    registerInputContext();
}

void MenuState::executeImpl() {
    engine::utils::printFPS<1>("Menu Update Rate", 50);
}

void MenuState::onEnterImpl() {
    gameData.componentManager->addComponent(menuEntity, menu);
    gameData.inputDispatcher->enableContext("menu-state");
    ResourceStorage& storage = *gameData.resourceStorage;
    storage.get<Music>("bgm-littleroot-town").play();
}

void MenuState::onExitImpl() {
    gameData.componentManager->removeComponent<Menu>(menuEntity);
    gameData.inputDispatcher->disableContext("menu-state");
    ResourceStorage &storage = *gameData.resourceStorage;
    storage.get<Music>("bgm-littleroot-town").stop();
}

void MenuState::registerInputContext() {
    InputContext context;

    static const auto hasMenu = [&] {
        return gameData.componentManager->hasComponent<Menu>(menuEntity);
    };

    static const auto getMenu = [&]() -> Menu& {
        return gameData.componentManager->getData<Menu>(menuEntity);
    };

    static const auto selectOption = [&] {
        if (hasMenu()) {
            Menu& menu = gameData.componentManager->getData<Menu>(menuEntity);
            menu.select();
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
