#ifndef SCRIPT_VARIABLES_HPP
#define SCRIPT_VARIABLES_HPP

#include <vector>
#include "../engine/entity-system/types.hpp"
#include "../engine/scripting-system/forward-declarations.hpp"

struct Battle;
struct CoreStructures;

/**
 * \brief Handles the script variables that are used by battle-related scripts.
 */
class ScriptVariables {
    using Entity = engine::entitysystem::Entity;
    using Lua = engine::scriptingsystem::Lua;
 public:
    ScriptVariables() = default;
    explicit ScriptVariables(CoreStructures& gameData);

    /**
     * \brief Sets the internal battle instance.
     */
    void setBattle(Battle&);

    /**
     * \brief Updates the universal script variables.
     */
    void updateScriptVariables();

    /**
     * \brief Updates the "user" script pointer.
     */
    void updateScriptUserPointer(Entity user);

    /**
     * \brief Updates the "target" script pointer.
     */
    void updateScriptTargetPointer(Entity target);

 private:
    Battle* battle;
    CoreStructures* gameData;

    size_t getPokemonIndex(Entity);
    void updateScriptVariables(const std::vector<Entity>& pokemonList, Lua&);
};

#endif
