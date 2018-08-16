#include "battle/events/ActionSelectionEvent.hpp"

#include "battle/Pokemon.hpp"
#include "components/BattleActionSelection.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

using engine::inputsystem::InputContext;

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
        {"Left", [&] { if (*focusedOption % 2 == 1) --*focusedOption; }},
        {"Right", [&] { if (*focusedOption % 2 == 0) ++*focusedOption; }},
        {"Up", [&] { if (*focusedOption >= 2) *focusedOption -= 2; }},
        {"Down", [&] { if (*focusedOption <= 1) *focusedOption += 2; }}
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

    focusedOption = &data<BattleActionSelection>(battle, gameData).focusedOption;
    selected = false;
}

bool ActionSelectionEvent::tickImpl() {
    if (selected) {
        *selectedOption = *focusedOption;
        disableInputContext("battle-action-selection", gameData);
        removeComponent<BattleActionSelection>(battle, gameData);
        focusedOption = nullptr;
        return true;
    }

    return false;
}
