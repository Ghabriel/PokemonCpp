import { Move, Pokemon } from "./types";

export const defaultEntityIds = {
    'user': -1,
    'target': -2,
    'userTeam': {
        0: -3,
        1: -4,
        2: -5,
        3: -6,
        4: -7,
    },
    'targetTeam': {
        0: -8,
        1: -9,
        2: -10,
        3: -11,
        4: -12,
    },
    'move': -13
};

export const user: Pokemon = { 'id': defaultEntityIds.user } as any;
export const target: Pokemon = { 'id': defaultEntityIds.target } as any;
export const userTeam = {
    0: { 'id': defaultEntityIds.userTeam[0] } as Pokemon,
    1: { 'id': defaultEntityIds.userTeam[1] } as Pokemon,
    2: { 'id': defaultEntityIds.userTeam[2] } as Pokemon,
    3: { 'id': defaultEntityIds.userTeam[3] } as Pokemon,
    4: { 'id': defaultEntityIds.userTeam[4] } as Pokemon,
};
export const targetTeam = {
    0: { 'id': defaultEntityIds.targetTeam[0] } as Pokemon,
    1: { 'id': defaultEntityIds.targetTeam[1] } as Pokemon,
    2: { 'id': defaultEntityIds.targetTeam[2] } as Pokemon,
    3: { 'id': defaultEntityIds.targetTeam[3] } as Pokemon,
    4: { 'id': defaultEntityIds.targetTeam[4] } as Pokemon,
};
export const move: Move = { 'pokemonId': defaultEntityIds.move } as any;
