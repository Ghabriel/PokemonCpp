#ifndef ACTION_SELECTION_EVENT_HPP
#define ACTION_SELECTION_EVENT_HPP

#include "../../engine/entity-system/types.hpp"
#include "../../events/Event.hpp"

struct CoreStructures;
struct Pokemon;

class ActionSelectionEvent : public Event {
    using Entity = engine::entitysystem::Entity;
 public:
    ActionSelectionEvent(
        const Pokemon& currentPokemon,
        Entity battle,
        CoreStructures& gameData
    );

 private:
    const Pokemon& currentPokemon;
    Entity battle;
    CoreStructures& gameData;
    size_t* focusedOption;
    bool selected;

    void registerInputContext();
    void onStartImpl() override;
    bool tickImpl() override;
};


#endif
