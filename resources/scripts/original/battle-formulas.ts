import { Stat } from './enums';
import { external, Pokemon } from './types';

export namespace specialized {
    export function getEffectiveStat(pokemon: Pokemon, stat: Stat, baseValue: number): number {
        if (stat == Stat.Speed && external.hasFlag(pokemon.id, 'Paralysis')) {
            baseValue *= 0.5;
        }

        return baseValue;
    }

    /**
     * Given the index of a move that has enough PP, checks if it can be used. Moves
     * like Disable, items like Choice Band, etc should be considered.
     */
    export function canUseMove(pokemon: Pokemon, moveIndex: number): boolean {
        // TODO
        return true;
    }
}
