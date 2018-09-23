import { NullState } from './NullState';
import { State } from './State';

export class StateMachine {
    constructor() {
        const nullState = new NullState();
        this.states.push(nullState);
        this.registeredStates.set('', nullState);
    }

    clear(): void {
        this.states = [];

        this.registeredStates.forEach((_, stateName) => {
            if (stateName !== '') {
                this.registeredStates.delete(stateName);
            }
        });
    }

    registerState(name: string, state: State): void {
        this.registeredStates.set(name, state);
    }

    pushState(stateName: string): void {
        this.states[this.states.length - 1].onExit();
        this.states.push(this.getStateByName(stateName));
        this.states[this.states.length - 1].onEnter();
    }

    popState(): void {
        if (this.states.length > 1) {
            this.states[this.states.length - 1].onExit();
            this.states.pop();
            this.states[this.states.length - 1].onEnter();
        }
    }

    private getStateByName(stateName: string): State {
        const state = this.registeredStates.get(stateName);

        if (!state) {
            throw Error('Invalid state: ' + stateName);
        }

        return state;
    }

    private states: State[] = [];
    private registeredStates = new Map<string, State>();
}
