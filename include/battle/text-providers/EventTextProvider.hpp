#ifndef EVENT_TEXT_PROVIDER_HPP
#define EVENT_TEXT_PROVIDER_HPP

#include "../../engine/resource-system/include.hpp"
#include "../../events/TextEvent.hpp"
#include "../TextProvider.hpp"
#include "CoreStructures.hpp"
#include "EventQueue.hpp"

class EventTextProvider : public TextProvider {
    using Entity = engine::entitysystem::Entity;
 private:
    void showBattleTextImpl(
        const std::string& content,
        Entity battleEntity,
        CoreStructures& gameData
    ) override {
        EventQueue& queue = gameData.resourceStorage->get<EventQueue>("battle-event-queue");
        queue.addEvent(std::make_unique<TextEvent>(content, battleEntity, gameData));
    }

    void showMoveTextImpl(
        const std::string& content,
        Entity battleEntity,
        CoreStructures& gameData
    ) override {
        EventQueue& queue = gameData.resourceStorage->get<EventQueue>("move-event-queue");
        queue.addEvent(std::make_unique<TextEvent>(content, battleEntity, gameData));
    }
};

#endif
