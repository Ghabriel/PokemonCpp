import { KeyboardKey } from './engine/input-system/KeyboadKey';
import { RawInput } from './engine/input-system/RawInput';

const charCodes: { [key: string]: number } = {
    'A': 65,
    'B': 66,
    'C': 67,
    'D': 68,
    'E': 69,
    'F': 70,
    'G': 71,
    'H': 72,
    'I': 73,
    'J': 74,
    'K': 75,
    'L': 76,
    'M': 77,
    'N': 78,
    'O': 79,
    'P': 80,
    'Q': 81,
    'R': 82,
    'S': 83,
    'T': 84,
    'U': 85,
    'V': 86,
    'W': 87,
    'X': 88,
    'Y': 89,
    'Z': 90,
    'Escape': 27,
    'Space': 32,
    'Return': 13,
    'Left': 37,
    'Right': 38,
    'Up': 39,
    'Down': 40,
};

export class RawInputBrowser implements RawInput {
    isKeyPressed(key: KeyboardKey): boolean {
        const keyName = KeyboardKey[key];

        if (!charCodes.hasOwnProperty(keyName)) {
            console.log('Warning: key detection not implemented for key: ' + keyName);
            return false;
        }

        return this.pressedKeys.has(charCodes[keyName]);
    }

    setupKeyWatcher(): void {
        window.addEventListener('keydown', event => {
            this.pressedKeys.add(event.keyCode);
        });

        window.addEventListener('keyup', event => {
            this.pressedKeys.delete(event.keyCode);
        });
    }

    private pressedKeys = new Set<number>();
}
