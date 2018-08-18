#ifndef INPUT_SYSTEM_INPUT_DISPATCHER_HPP
#define INPUT_SYSTEM_INPUT_DISPATCHER_HPP

#include <set>
#include <string>
#include <unordered_set>
#include "InputContext.hpp"
#include "InputTracker.hpp"
#include "types.hpp"

namespace engine::inputsystem {
    namespace __detail {
        struct InputContextPointerCompare {
            constexpr bool operator()(
                const InputContext* lhs,
                const InputContext* rhs
            ) const {
                return lhs->priority > rhs->priority;
            }
        };
    }

    /**
     * \brief Manages the active input contexts and triggers their callbacks
     * at the appropriate moments.
     */
    class InputDispatcher {
        using InputContextPointerCompare = __detail::InputContextPointerCompare;
     public:
        explicit InputDispatcher(InputTracker&);

        /**
         * \brief Clears the enabled contexts and all registered contexts. This
         * instance becomes effectively the same as when it was constructed.
         */
        void clear();

        /**
         * \brief Should be called at every update tick. Updates the
         * internal structures and dispatches appropriate callbacks.
         */
        void tick();

        /**
         * \brief Registers a context, mapping it to a given name. If the given
         * name is already registered, its corresponding context is replaced.
         */
        void registerContext(const std::string& name, const InputContext&);
        /**
         * \brief Enables a context, given its name. Throws if it's not
         * a registered name.
         */
        void enableContext(const std::string& contextName);
        /**
         * \brief Disables a context, given its name. Throws if it's not
         * a registered name.
         */
        void disableContext(const std::string& contextName);
        /**
         * \brief Disables all the enabled contexts.
         */
        void disableAllContexts();
    
     private:
        InputTracker& inputTracker;
        std::set<const InputContext*, InputContextPointerCompare> activeContexts;
        std::unordered_map<std::string, InputContext> registeredContexts;
    };

    inline InputDispatcher::InputDispatcher(InputTracker& tracker) : inputTracker(tracker) {}

    inline void InputDispatcher::clear() {
        activeContexts.clear();
        registeredContexts.clear();
    }

    inline void InputDispatcher::tick() {
        inputTracker.tick();

        inputTracker.forEachActionKey([&](const GameKey& key) {
            for (const InputContext* context : activeContexts) {
                if (context->actions.count(key)) {
                    context->actions.at(key)();
                    break;
                }
            }
        });

        inputTracker.forEachStateKey([&](const GameKey& key) {
            for (const InputContext* context : activeContexts) {
                if (context->states.count(key)) {
                    context->states.at(key)();
                    break;
                }
            }
        });
    }

    inline void InputDispatcher::registerContext(
        const std::string& name,
        const InputContext& context
    ) {
        registeredContexts.insert_or_assign(name, context);
    }

    inline void InputDispatcher::enableContext(const std::string& contextName) {
        activeContexts.insert(&registeredContexts.at(contextName));
    }

    inline void InputDispatcher::disableContext(const std::string& contextName) {
        activeContexts.erase(&registeredContexts.at(contextName));
    }

    inline void InputDispatcher::disableAllContexts() {
        activeContexts.clear();
    }
}

#endif
