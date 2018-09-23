import { InputTracker } from '../engine/input-system/InputTracker';
import { KeyboardKey } from '../engine/input-system/KeyboadKey';
import { GameKey } from '../engine/input-system/types';
import { RawInputBrowser } from '../RawInputBrowser';

interface ControlsMap {
    [keyboardKey: string]: string;
}

export function loadInputTracker(controlsMap: ControlsMap): InputTracker {
    const rawInput = new RawInputBrowser();
    rawInput.setupKeyWatcher();
    const keyMapping = buildKeyMapping(controlsMap);
    return new InputTracker(rawInput, keyMapping);
}

function buildKeyMapping(controlsMap: ControlsMap): Map<KeyboardKey, GameKey> {
    const keyMapping = new Map<KeyboardKey, GameKey>();

    for (const keyboardKey in controlsMap) {
        const gameKey = controlsMap[keyboardKey];
        keyMapping.set(KeyboardKey[keyboardKey as keyof typeof KeyboardKey], gameKey);
    }

    return keyMapping;
}

