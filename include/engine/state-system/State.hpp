#ifndef STATE_SYSTEM_STATE_HPP
#define STATE_SYSTEM_STATE_HPP

namespace engine::statesystem {
    class State {
    public:
        virtual ~State() = default;

        void execute() {
            executeImpl();
        }

        void onEnter() {
            onEnterImpl();
        }

        void onExit() {
            onExitImpl();
        }

    private:
        virtual void executeImpl() = 0;
        virtual void onEnterImpl() { }
        virtual void onExitImpl() { }
    };
}

#endif
