import { Gender } from './Gender';
import { Nature } from './Nature';
import { StatusCondition } from './StatusCondition';

export interface Pokemon {
    species: string;
    nature: Nature;
    heldItem: string;
    experiencePoints: number;
    ability: string;
    ev: number[];
    iv: number[];
    moves: string[];
    currentPP: number[];
    ppUps: number[];
    eggStepsToHatch: number; // < 0 if not an egg
    gender: Gender;
    form: number;
    isNicknamed: boolean;
    displayName: string;
    metAtDate: number;
    metAtLocation: string;
    metAtLevel: number;
    pokerus: boolean;
    pokeball: string;

    // Battle stats
    statusCondition: StatusCondition;
    asleepRounds: number;
    level: number;
    stats: number[];
    currentHP: number;
}
