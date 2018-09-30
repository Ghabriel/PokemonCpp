import { Stat, StatFlag } from './enums';
import { specialized } from './new-battle-formulas';
import { external, luaImplicitSelf, Move, Pokemon } from './types';

const typeTable = [
    [ 1, 1, 1, 1, 1, 0.5, 1, 0, 0.5, 1, 1, 1, 1, 1, 1, 1, 1, 1 ],
    [ 2, 1, 0.5, 0.5, 1, 2, 0.5, 0, 2, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5 ],
    [ 1, 2, 1, 1, 1, 0.5, 2, 1, 0.5, 1, 1, 2, 0.5, 1, 1, 1, 1, 1 ],
    [ 1, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 0, 1, 1, 2, 1, 1, 1, 1, 1, 2 ],
    [ 1, 1, 0, 2, 1, 2, 0.5, 1, 2, 2, 1, 0.5, 2, 1, 1, 1, 1, 1 ],
    [ 1, 0.5, 2, 1, 0.5, 1, 2, 1, 0.5, 2, 1, 1, 1, 1, 2, 1, 1, 1 ],
    [ 1, 0.5, 0.5, 0.5, 1, 1, 1, 0.5, 0.5, 0.5, 1, 2, 1, 2, 1, 1, 2, 0.5 ],
    [ 0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 1 ],
    [ 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 1, 2, 1, 1, 2 ],
    [ 1, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5, 0.5, 2, 1, 1, 2, 0.5, 1, 1 ],
    [ 1, 1, 1, 1, 2, 2, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 1, 0.5, 1, 1 ],
    [ 1, 1, 0.5, 0.5, 2, 2, 0.5, 1, 0.5, 0.5, 2, 0.5, 1, 1, 1, 0.5, 1, 1 ],
    [ 1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 0.5, 1, 1 ],
    [ 1, 2, 1, 2, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 0.5, 1, 1, 0, 1 ],
    [ 1, 1, 2, 1, 2, 1, 1, 1, 0.5, 0.5, 0.5, 2, 1, 1, 0.5, 2, 1, 1 ],
    [ 1, 1, 1, 1, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 1, 1, 2, 1, 0 ],
    [ 1, 0.5, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5 ],
    [ 1, 2, 1, 0.5, 1, 1, 1, 1, 0.5, 0.5, 1, 1, 1, 1, 1, 2, 2, 1 ],
];

const typeMapping = {
    'Normal': 1,
    'Fighting': 2,
    'Flying': 3,
    'Poison': 4,
    'Ground': 5,
    'Rock': 6,
    'Bug': 7,
    'Ghost': 8,
    'Steel': 9,
    'Fire': 10,
    'Water': 11,
    'Grass': 12,
    'Electric': 13,
    'Psychic': 14,
    'Ice': 15,
    'Dragon': 16,
    'Dark': 17,
    'Fairy': 18,
};

luaImplicitSelf(specialized);

export function getStatStageMultiplier(stage: number): number {
    const absStage = Math.abs(stage);

    if (stage >= 0) {
        return (2 + absStage) / 2;
    } else {
        return 2 / (2 + absStage);
    }
}

export function getAccuracyStatStageMultiplier(stage: number): number {
    const absStage = Math.abs(stage);

    if (stage >= 0) {
        return (3 + absStage) / 3;
    } else {
        return 3 / (3 + absStage);
    }
}

export function getAttackStatForMove(
    pokemon: Pokemon,
    move: Move,
    calculationFlags: StatFlag
): number {
    if (move.kind === 'Physical') {
        return getEffectiveStat(pokemon, Stat.Attack, calculationFlags);
    }

    if (move.kind === 'Special') {
        return getEffectiveStat(pokemon, Stat.SpecialAttack, calculationFlags);
    }

    return 0;
}

export function getDefenseStatForMove(
    pokemon: Pokemon,
    move: Move,
    calculationFlags: StatFlag
): number {
    if (move.kind === 'Physical') {
        return getEffectiveStat(pokemon, Stat.Defense, calculationFlags);
    }

    if (move.kind === 'Special') {
        return getEffectiveStat(pokemon, Stat.SpecialDefense, calculationFlags);
    }

    return 0;
}

export function getEffectiveStat(
    pokemon: Pokemon,
    stat: Stat,
    calculationFlags: StatFlag
): number {
    const standardStatValue = getStandardStat(pokemon, stat);
    const currentStage = getModifiedStatStage(pokemon, stat, calculationFlags);
    const statValue = standardStatValue * getStatStageMultiplier(currentStage);
    return specialized.getEffectiveStat(pokemon, stat, statValue);
}

export function getStandardStat(pokemon: Pokemon, stat: Stat): number {
    return external.getStandardStat(pokemon.id, stat);
}

export function getModifiedStatStage(
    pokemon: Pokemon,
    stat: Stat,
    calculationFlags: StatFlag
): number {
    let currentStage = getStatStage(pokemon, stat);

    if (calculationFlags === StatFlag.IgnorePositive) {
        currentStage = Math.min(0, currentStage);
    } else if (calculationFlags === StatFlag.IgnoreNegative) {
        currentStage = Math.max(0, currentStage);
    }

    return currentStage;
}

export function getStatStage(pokemon: Pokemon, stat: Stat): number {
    return external.getStatStage(pokemon.id, stat);
}

export function hasUsableMoves(pokemon: Pokemon): boolean {
    // TODO
    return true;
}

export function canUseMove(pokemon: Pokemon, moveIndex: number): boolean {
    return getPP(pokemon, moveIndex) > 0 && specialized.canUseMove(pokemon, moveIndex);
}

export function getPP(pokemon: Pokemon, moveIndex: number): number {
    if (moveIndex === 0) {
        return pokemon.pp0;
    } else if (moveIndex === 1) {
        return pokemon.pp1;
    } else if (moveIndex === 2) {
        return pokemon.pp2;
    } else if (moveIndex === 3) {
        return pokemon.pp3;
    }

    return 0
}

export function checkMiss(user: Pokemon, target: Pokemon, move: Move): boolean {
    if (move.accuracy === 0) {
        return false;
    }

    const userAccuracyStage = getStatStage(user, Stat.Accuracy);
    const targetEvasionStage = getStatStage(target, Stat.Evasion);
    const accuracyStage = clamp(userAccuracyStage - targetEvasionStage, -6, 6);
    const accuracyStageMultiplier = getAccuracyStatStageMultiplier(accuracyStage);
    const hitRate = move.accuracy * accuracyStageMultiplier;
    external.log('Hit rate: ' + hitRate);
    return random(1, 100) > hitRate
}

export function clamp(value: number, minValue: number, maxValue: number): number {
    return Math.max(minValue, Math.min(maxValue, value));
}

export function checkCritical(user: Pokemon, target: Pokemon, move: Move): boolean {
    const criticalHitStage = getCriticalHitStage(user);
    let chancesIn24: number;

    if (criticalHitStage === 0) {
        chancesIn24 = 1;
    } else if (criticalHitStage === 1) {
        chancesIn24 = 3;
    } else if (criticalHitStage === 2) {
        chancesIn24 = 12;
    } else {
        chancesIn24 = 24;
    }

    return random(1, 24) <= chancesIn24;
}

export function getCriticalHitStage(pokemon: Pokemon): number {
    // TODO
    return 0;
}

export function random(min: number, max: number): number {
    return external.random(min, max);
}

export function getTypeEffectiveness(pokemon: Pokemon, move: Move): number {
    if (move.type === '???') {
        return 1;
    }

    const moveTypeIndex = typeMapping[move.type as keyof typeof typeMapping];
    const firstTypeIndex = typeMapping[pokemon.type0 as keyof typeof typeMapping];
    let result = typeTable[moveTypeIndex][firstTypeIndex];

    if (pokemon.typeCount > 1) {
        const secondTypeIndex = typeMapping[pokemon.type1 as keyof typeof typeMapping];
        result *= typeTable[moveTypeIndex][secondTypeIndex];
    }

    return result
}

export function calculateExpGain(winner: Pokemon, fainted: Pokemon) {
    const luckyEgg = 1; // TODO: handle Lucky Egg
    const shareFactor = 1; // TODO: handle Exp.Share
    const factor1 = (fainted.baseExp * fainted.level) / (5 * shareFactor);
    const factor2 = Math.pow(2 * fainted.level + 10, 2.5);
    const factor3 = Math.pow(fainted.level + winner.level + 10, 2.5);
    return (factor1 * (factor2 / factor3) + 1) * luckyEgg;
}
