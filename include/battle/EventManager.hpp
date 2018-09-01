#ifndef EVENT_MANAGER_HPP
#define EVENT_MANAGER_HPP

#include <string>
#include <unordered_set>
#include "../engine/entity-system/types.hpp"
#include "../engine/scripting-system/forward-declarations.hpp"

struct Battle;
struct BoundFlag;
struct CoreStructures;
struct ScriptVariables;
struct UsedMove;

/**
 * \brief Handles the correct dispatching of battle events.
 */
class EventManager {
    using Entity = engine::entitysystem::Entity;
    using Lua = engine::scriptingsystem::Lua;
 public:
    EventManager(ScriptVariables&, CoreStructures& gameData);

    /**
     * \brief Sets the internal battle instance.
     */
    void setBattle(Battle&);

    /**
     * \brief Triggers the specified event on all active moves, flags, and teams.
     */
    void triggerEvent(const std::string& eventName);

    /**
     * \brief Triggers the specified event on the specified move.
     */
    void triggerMoveEvent(const UsedMove&, const std::string& eventName);

    /**
     * \brief Triggers the specified event on the specified bound flag.
     */
    void triggerFlagEvent(const BoundFlag&, const std::string& eventName);

    /**
     * \brief Triggers the specified event on all flags of the target,
     * including its team's.
     */
    void triggerTargetFlagEvent(Entity target, const std::string& eventName);

 private:
    ScriptVariables& scriptVariables;
    CoreStructures& gameData;
    Battle* battle;

    std::unordered_set<std::string>& getVolatileFlags(Entity);
};

#endif
