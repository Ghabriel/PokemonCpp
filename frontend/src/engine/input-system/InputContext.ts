import { GameKey } from './types';

export interface InputContext {
    actions: Map<GameKey, () => void>;
    states: Map<GameKey, () => void>;
    priority: number;
}
