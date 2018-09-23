import { KeyboardKey } from './KeyboadKey';
import { RawInput } from './RawInput';
import { GameKey } from './types';

export class InputTracker {
    constructor(
        private rawInput: RawInput,
        private relevantKeys: Map<KeyboardKey, GameKey>
    ) { }

    tick(): void {
        this.actions.clear();
        this.states.clear();

        this.relevantKeys.forEach((gameKey, keyboardKey) => {
            if (this.rawInput.isKeyPressed(keyboardKey)) {
                this.states.add(gameKey);

                if (!this.currentlyPressedKeys.has(keyboardKey)) {
                    this.actions.add(gameKey);
                }

                this.currentlyPressedKeys.add(keyboardKey);
            } else {
                this.currentlyPressedKeys.delete(keyboardKey);
            }
        });
    }

    forEachActionKey(fn: (gameKey: GameKey) => void): void {
        this.actions.forEach(fn);
    }

    forEachStateKey(fn: (gameKey: GameKey) => void): void {
        this.states.forEach(fn);
    }

    private currentlyPressedKeys = new Set<KeyboardKey>();
    private actions = new Set<GameKey>();
    private states = new Set<GameKey>();
}
