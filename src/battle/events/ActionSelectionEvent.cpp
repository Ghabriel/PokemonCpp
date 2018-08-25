#include "battle/events/ActionSelectionEvent.hpp"

#include "core-functions.hpp"
#include "CoreStructures.hpp"

using engine::entitysystem::Entity;
using engine::inputsystem::InputContext;

ActionSelectionEvent::ActionSelectionEvent(
    int& selectedOption,
    bool cancelable,
    std::function<size_t&()> getFocusedOption,
    std::function<bool(size_t)> isValidOption,
    CoreStructures& gameData
) : selectedOption(&selectedOption),
    cancelable(cancelable),
    getFocusedOption(getFocusedOption),
    isValidOption(isValidOption),
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
            size_t& focusedOption = getFocusedOption();
            if (focusedOption % 2 == 1) {
                sound("fx-select-option", gameData).play();
                --focusedOption;
            }
        }},
        {"Right", [&] {
            size_t& focusedOption = getFocusedOption();
            if (focusedOption % 2 == 0) {
                sound("fx-select-option", gameData).play();
                ++focusedOption;
            }
        }},
        {"Up", [&] {
            size_t& focusedOption = getFocusedOption();
            if (focusedOption >= 2) {
                sound("fx-select-option", gameData).play();
                focusedOption -= 2;
            }
        }},
        {"Down", [&] {
            size_t& focusedOption = getFocusedOption();
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
    state = SelectionState::Pending;
}

bool ActionSelectionEvent::tickImpl() {
    if (!isValidOption(getFocusedOption())) {
        state = SelectionState::Pending;
    }

    if (state == SelectionState::Pending) {
        return false;
    }

    sound("fx-select-option", gameData).play();
    *selectedOption = (state == SelectionState::Selected)
        ? getFocusedOption()
        : -1;
    disableInputContext("battle-action-selection", gameData);
    return true;
}
