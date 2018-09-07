#include "battle/EventManager.hpp"

#include "battle/data/BoundMove.hpp"
#include "battle/data/Flag.hpp"
#include "battle/data/Move.hpp"
#include "battle/helpers/move-effects.hpp"
#include "battle/ScriptVariables.hpp"
#include "components/battle/Battle.hpp"
#include "components/battle/VolatileData.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

namespace {
    template<typename TKey>
    void triggerFlagTableEvent(
        EventManager& eventManager,
        std::unordered_map<TKey, std::vector<Flag>>& flagTable,
        const std::string& eventName
    ) {
        for (auto& [_, flagVector] : flagTable) {
            for (auto& flag : flagVector) {
                eventManager.triggerFlagEvent(flag, eventName);
            }
        }
    }
}

EventManager::EventManager(ScriptVariables& variables, CoreStructures& gameData)
 : scriptVariables(&variables), gameData(&gameData) { }

void EventManager::setBattle(Battle& _battle) {
    battle = &_battle;
}

void EventManager::triggerEvent(const std::string& eventName) {
    for (const auto& boundMove : battle->usedMoves) {
        triggerMoveEvent(boundMove, eventName);
    }

    triggerFlagTableEvent(*this, battle->playerTeamPositionFlags, eventName);
    triggerFlagTableEvent(*this, battle->opponentTeamPositionFlags, eventName);
    triggerFlagTableEvent(*this, battle->pokemonFlags, eventName);
}

void EventManager::triggerUserEvents(const BoundMove& boundMove, const std::string& eventName) {
    triggerMoveEvent(boundMove, eventName);

    std::vector<Flag>& flags = battle->pokemonFlags[boundMove.user];
    for (const auto& flag : flags) {
        script("moves", *gameData).call<void>("Flag_" + flag.id + '_' + eventName);
    }
}

void EventManager::triggerMoveEvent(const BoundMove& boundMove, const std::string& eventName) {
    effects::internal::setMove(boundMove);
    script("moves", *gameData).call<void>(boundMove.move->id + '_' + eventName);
}

void EventManager::triggerFlagEvent(const Flag& flag, const std::string& eventName) {
    // XTRACE(flag.target);
    // XTRACE(flag.flag);
    // XTRACE(eventName);
    // ECHO("----------");
    effects::internal::setFlag(flag);
    // updateMoveVariables(9999999, target);
    scriptVariables->updateScriptTargetPointer(flag.target); // TODO: is this needed?
    script("moves", *gameData).call<void>("Flag_" + flag.id + '_' + eventName);
}
