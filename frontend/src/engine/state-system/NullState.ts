import { State } from './State';

export class NullState implements State {
    // tslint:disable:no-empty
    execute(): void { }
    onEnter(): void { }
    onExit(): void { }
}
