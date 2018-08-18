#include "battle/events/ActionSelectionEvent.hpp"

#include "battle/Pokemon.hpp"
#include "components/BattleActionSelection.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

using engine::entitysystem::Entity;
using engine::inputsystem::InputContext;

size_t& getFocusedOption(Entity battle, CoreStructures& gameData) {
    return data<BattleActionSelection>(battle, gameData).focusedOption;
}

ActionSelectionEvent::ActionSelectionEvent(
    int& selectedOption,
    bool cancelable,
    const Pokemon& currentPokemon,
    Entity battle,
    CoreStructures& gameData
) : selectedOption(&selectedOption),
    cancelable(cancelable),
    currentPokemon(currentPokemon),
    battle(battle),
    gameData(gameData) {
    registerInputContext();
}

void ActionSelectionEvent::registerInputContext() {
    InputContext context;

    context.actions = {
        {"Action", [&] { state = SelectionState::Selected; }},
        {"Cancel", [&] {
            if (cancelable) {
                state = SelectionState::Canceled;
            }
        }},
        {"Left", [&] {
            size_t& focusedOption = getFocusedOption(battle, gameData);
            if (focusedOption % 2 == 1) {
                sound("fx-select-option", gameData).play();
                --focusedOption;
            }
        }},
        {"Right", [&] {
            size_t& focusedOption = getFocusedOption(battle, gameData);
            if (focusedOption % 2 == 0) {
                sound("fx-select-option", gameData).play();
                ++focusedOption;
            }
        }},
        {"Up", [&] {
            size_t& focusedOption = getFocusedOption(battle, gameData);
            if (focusedOption >= 2) {
                sound("fx-select-option", gameData).play();
                focusedOption -= 2;
            }
        }},
        {"Down", [&] {
            size_t& focusedOption = getFocusedOption(battle, gameData);
            if (focusedOption <= 1) {
                sound("fx-select-option", gameData).play();
                focusedOption += 2;
            }
        }}
    };

    context.priority = 1;
    gameData.inputDispatcher->registerContext("battle-action-selection", context);
}

void ActionSelectionEvent::onStartImpl() {
    enableInputContext("battle-action-selection", gameData);

    addComponent(
        battle,
        BattleActionSelection{
            "What will " + currentPokemon.displayName + " do?",
            300,
            {"Fight", "Bag", "Pokemon", "Run"},
            0
        },
        gameData
    );

    state = SelectionState::Pending;
}

bool ActionSelectionEvent::tickImpl() {
    if (state == SelectionState::Pending) {
        return false;
    }

    *selectedOption = (state == SelectionState::Selected)
        ? getFocusedOption(battle, gameData)
        : -1;
    disableInputContext("battle-action-selection", gameData);
    removeComponent<BattleActionSelection>(battle, gameData);
    return true;
}
