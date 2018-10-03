export declare function luaImplicitSelf(namespace: any): void;
export declare function luaImport(path: string): void;

export declare namespace external {
    function log(content: string): void;

    function damage(): void;
    function damageWithFixedRecoil(lostHP: number): void;
    function damageWithRecoil(recoilRate: number): void;
    function fixedDamage(lostHP: number): void;
    function lowerStat(statId: number, levels: number): void;
    function raiseStat(statId: number, levels: number): void;
    function ensureCriticalHit(): void;
    function multiplyDamage(factor: number): void;
    function negateMove(): void;
    function addStatusCondition(entityId: number, statusConditionId: number): void;
    function removeStatusCondition(entityId: number): void;
    function sleep(entityId: number, duration: number): void;
    function reduceSleepCounter(): void;

    // Flag-related functions
    function addTimedFlagUser(flagId: string, duration: number): void;
    function addFlagUser(flagId: string): void;
    function addTimedFlagTarget(flagId: string, duration: number): void;
    function addFlagTarget(flagId: string): void;
    function removeFlagUser(flagId: string): void;
    function removeFlagTarget(flagId: string): void;
    function hasFlag(entityId: number, flagId: string): boolean;

    // Getters
    function getStandardStat(entityId: number, statId: number): number;
    function getStatStage(entityId: number, statId: number): number;

    function random(min: number, max: number): number;
    function showText(content: string): void;
}

export interface Pokemon {
    relativeId: number;
    id: number;
    move0: string;
    move1: string;
    move2: string;
    move3: string;
    pp0: number;
    pp1: number;
    pp2: number;
    pp3: number;
    species: string;
    nature: string;
    heldItem: string;
    experiencePoints: number;
    ability: string;
    moveCount: number;
    gender: string; // TODO: enum
    form: number;
    displayName: string;
    pokerus: boolean;
    statusCondition: string; // TODO: enum
    asleepRounds: number;
    level: number;
    hp: number;
    attack: number;
    defense: number;
    specialAttack: number;
    specialDefense: number;
    speed: number;
    currentHP: number;
    typeCount: number;
    type0: string;
    type1: string;
    baseExp: number;
    captureRate: number;
    height: number;
    weight: number;
}

export interface Move {
    id: number;
    displayName: string;
    type: string;
    kind: 'Physical' | 'Special' | 'Status';
    power: number;
    accuracy: number;
    currentPP: number;
    targetType: string;
    priority: number;
    flags: string; // TODO
}
