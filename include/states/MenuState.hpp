#ifndef MENU_STATE_HPP
#define MENU_STATE_HPP

#include "../engine/entity-system/types.hpp"
#include "../engine/state-system/include.hpp"
#include "../engine/utils/misc/Menu.hpp"

struct CoreStructures;

class MenuState : public engine::statesystem::State {
    using Entity = engine::entitysystem::Entity;
 public:
    MenuState(CoreStructures& gameData);

 private:
    CoreStructures& gameData;
    engine::utils::Menu menu;
    engine::entitysystem::Entity menuEntity;

    void executeImpl() override;
    void onEnterImpl() override;
    void onExitImpl() override;
    void registerInputContext();
};

#endif
