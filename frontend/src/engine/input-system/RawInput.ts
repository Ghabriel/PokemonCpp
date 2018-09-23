import { KeyboardKey } from './KeyboadKey';

export interface RawInput {
    isKeyPressed(key: KeyboardKey): boolean;
}
