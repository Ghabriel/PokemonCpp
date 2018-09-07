#ifndef EVENT_MANAGER_HPP
#define EVENT_MANAGER_HPP

#include <string>
#include <unordered_set>
#include "../engine/entity-system/types.hpp"
#include "../engine/scripting-system/forward-declarations.hpp"

struct Battle;
struct BoundMove;
struct CoreStructures;
struct Flag;
struct ScriptVariables;

/**
 * \brief Handles the correct dispatching of battle events.
 */
class EventManager {
    using Entity = engine::entitysystem::Entity;
    using Lua = engine::scriptingsystem::Lua;
 public:
    EventManager() = default;
    EventManager(ScriptVariables&, CoreStructures& gameData);

    /**
     * \brief Sets the internal battle instance.
     */
    void setBattle(Battle&);

    /**
     * \brief Triggers the specified event on all active moves and flags.
     */
    void triggerEvent(const std::string& eventName);

    /**
     * \brief Triggers the specified event on the given move and all flags
     * that are bound to its user.
     */
    void triggerUserEvents(const BoundMove&, const std::string& eventName);

    /**
     * \brief Triggers the specified event on the given flag.
     */
    void triggerFlagEvent(const Flag&, const std::string& eventName);

 private:
    ScriptVariables* scriptVariables;
    CoreStructures* gameData;
    Battle* battle;

    /**
     * \brief Triggers the specified event on the specified move.
     */
    void triggerMoveEvent(const BoundMove&, const std::string& eventName);
};

#endif
