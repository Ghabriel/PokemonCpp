import { InputContext } from './InputContext';
import { InputTracker } from './InputTracker';

export class InputDispatcher {
    constructor(private inputTracker: InputTracker) { }

    clear(): void {
        this.activeContexts.clear();
        this.registeredContexts.clear();
    }

    tick(): void {
        this.inputTracker.tick();
        this.callMatchingActions();
        this.callMatchingStates();
    }

    registerContext(name: string, context: InputContext): void {
        this.registeredContexts.set(name, context);
    }

    enableContext(contextName: string): void {
        this.activeContexts.add(this.getContextByName(contextName));
    }

    disableContext(contextName: string): void {
        this.activeContexts.delete(this.getContextByName(contextName));
    }

    disableAllContexts(): void {
        this.activeContexts.clear();
    }

    private callMatchingActions() {
        this.inputTracker.forEachActionKey(gameKey => {
            for (const context of this.activeContexts) {
                if (context.actions.has(gameKey)) {
                    context.actions.get(gameKey)!();
                    break;
                }
            }
        });
    }

    private callMatchingStates() {
        this.inputTracker.forEachStateKey(gameKey => {
            for (const context of this.activeContexts) {
                if (context.states.has(gameKey)) {
                    context.states.get(gameKey)!();
                    break;
                }
            }
        });
    }

    private getContextByName(contextName: string): InputContext {
        const context = this.registeredContexts.get(contextName);

        if (!context) {
            throw Error('Invalid context: ' + contextName);
        }

        return context;
    }

    private activeContexts = new Set<InputContext>();
    private registeredContexts = new Map<string, InputContext>();
}
