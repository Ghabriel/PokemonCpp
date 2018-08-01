#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "../engine/entity-system/include.hpp"
#include "../engine/resource-system/ResourceStorage.hpp"
#include "../engine/sfml/sound-system/Music.hpp"
#include "../engine/state-system/State.hpp"
#include "../engine/utils/misc/Menu.hpp"
#include "../engine/utils/timing/print-fps.hpp"
#include "../GameCoreData.hpp"

class MenuState : public engine::statesystem::State {
    using Entity = engine::entitysystem::Entity;
    using Menu = engine::utils::Menu;
    using Music = engine::soundsystem::Music;
    using ResourceStorage = engine::resourcesystem::ResourceStorage;
 public:
    MenuState(GameCoreData& gameData)
     : gameData(gameData),
       menuEntity(gameData.componentManager->createEntity()) {
        Menu menu(*gameData.stateMachine);
        menu.addOption("New Game", "menu-state");
        menu.addOption("Load Game", "menu-state");
        menu.addOption("About", "menu-state");

        gameData.componentManager->addComponent(menuEntity, menu);
    }

 private:
    GameCoreData& gameData;
    Entity menuEntity;

    void executeImpl() override {
        engine::utils::printFPS<1>("Menu Update Rate", 50);
    }

    void onEnterImpl() override {
        ResourceStorage& storage = *gameData.resourceStorage;
        storage.get<Music>("bgm-littleroot-town").play();
    }

    void onExitImpl() override {
        ResourceStorage &storage = *gameData.resourceStorage;
        storage.get<Music>("bgm-littleroot-town").stop();
    }
};

#endif
