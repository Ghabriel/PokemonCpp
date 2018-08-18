#ifndef ACTION_SELECTION_EVENT_HPP
#define ACTION_SELECTION_EVENT_HPP

#include "../../engine/entity-system/types.hpp"
#include "../../events/Event.hpp"

struct CoreStructures;
struct Pokemon;

enum class SelectionState {
    Selected,
    Canceled,
    Pending
};

class ActionSelectionEvent : public Event {
    using Entity = engine::entitysystem::Entity;
 public:
    ActionSelectionEvent(
        int& selectedOption,
        bool cancelable,
        const Pokemon& currentPokemon,
        Entity battle,
        CoreStructures& gameData
    );

 private:
    int* selectedOption;
    bool cancelable;
    const Pokemon& currentPokemon;
    Entity battle;
    CoreStructures& gameData;
    SelectionState state;

    void registerInputContext();
    void onStartImpl() override;
    bool tickImpl() override;
};


#endif
