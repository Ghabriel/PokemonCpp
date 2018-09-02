#include "battle/EventManager.hpp"

#include "battle/helpers/move-effects.hpp"
#include "battle/ScriptVariables.hpp"
#include "components/battle/Battle.hpp"
#include "components/battle/VolatileData.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

EventManager::EventManager(ScriptVariables& variables, CoreStructures& gameData)
 : scriptVariables(&variables), gameData(&gameData) { }

void EventManager::triggerEvent(const std::string& eventName) {
    for (const auto& [usedMove, _] : battle->activeMoves) {
        triggerMoveEvent(usedMove, eventName);
    }

    for (const auto& usedMove : battle->usedMoves) {
        triggerMoveEvent(usedMove, eventName);
    }

    triggerTargetFlagEvent(battle->playerPokemon, eventName);
    triggerTargetFlagEvent(battle->opponentPokemon, eventName);
}

void EventManager::setBattle(Battle& _battle) {
    battle = &_battle;
}

void EventManager::triggerMoveEvent(const UsedMove& usedMove, const std::string& eventName) {
    effects::internal::setMoveUser(usedMove.user);
    effects::internal::setMoveTarget(usedMove.target);
    effects::internal::setMove(*usedMove.move);
    effects::internal::setUsedMove(usedMove);
    script("moves", *gameData).call<void>(usedMove.move->id + '_' + eventName);
}

void EventManager::triggerFlagEvent(const BoundFlag& boundFlag, const std::string& eventName) {
    // XTRACE(boundFlag.target);
    // XTRACE(boundFlag.flag);
    // XTRACE(eventName);
    // ECHO("----------");
    effects::internal::setMoveTarget(boundFlag.target);
    // updateMoveVariables(9999999, target);
    scriptVariables->updateScriptTargetPointer(boundFlag.target);
    script("moves", *gameData).call<void>("Flag_" + boundFlag.flag + '_' + eventName);
}

void EventManager::triggerTargetFlagEvent(Entity target, const std::string& eventName) {
    if (target == battle->playerPokemon) {
        for (const auto& flag : battle->playerTeamFlags) {
            triggerFlagEvent({battle->playerPokemon, flag}, eventName);
        }

        for (const auto& flag : getVolatileFlags(battle->playerPokemon)) {
            triggerFlagEvent({battle->playerPokemon, flag}, eventName);
        }
    } else {
        for (const auto& flag : battle->opponentTeamFlags) {
            triggerFlagEvent({battle->opponentPokemon, flag}, eventName);
        }

        for (const auto& flag : getVolatileFlags(battle->opponentPokemon)) {
            triggerFlagEvent({battle->opponentPokemon, flag}, eventName);
        }
    }
}

std::unordered_set<std::string>& EventManager::getVolatileFlags(Entity entity) {
    return data<VolatileData>(entity, *gameData).flags;
}
