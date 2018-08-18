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
    size_t& selectedOption,
    const Pokemon& currentPokemon,
    Entity battle,
    CoreStructures& gameData
) : currentPokemon(currentPokemon),
    battle(battle),
    gameData(gameData),
    selectedOption(&selectedOption) {
    registerInputContext();
}

void ActionSelectionEvent::registerInputContext() {
    InputContext context;

    context.actions = {
        {"Action", [&] { selected = true; }},
        {"Cancel", [&] { std::cout << "Cancel\n"; }},
        {"Left", [&] {
            size_t& focusedOption = getFocusedOption(battle, gameData);
            if (focusedOption % 2 == 1) {
                --focusedOption;
            }
        }},
        {"Right", [&] {
            size_t& focusedOption = getFocusedOption(battle, gameData);
            if (focusedOption % 2 == 0) {
                ++focusedOption;
            }
        }},
        {"Up", [&] {
            size_t& focusedOption = getFocusedOption(battle, gameData);
            if (focusedOption >= 2) {
                focusedOption -= 2;
            }
        }},
        {"Down", [&] {
            size_t& focusedOption = getFocusedOption(battle, gameData);
            if (focusedOption <= 1) {
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

    selected = false;
}

bool ActionSelectionEvent::tickImpl() {
    if (selected) {
        *selectedOption = getFocusedOption(battle, gameData);
        disableInputContext("battle-action-selection", gameData);
        removeComponent<BattleActionSelection>(battle, gameData);
        return true;
    }

    return false;
}
