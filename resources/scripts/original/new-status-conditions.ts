import { random } from "./new-battle-utils";
import { move, target } from "./new-battle-variables";
import { external, Pokemon } from "./types";

/**
 * Burn - 90% (not checking faint)
 * Freeze - 100%
 * Paralysis - 100%
 * Poison - 90% (not checking faint)
 * Toxic - 80% (onSwitchIn and onBattleEnd are untestable at the moment)
 * Sleep - 100%
 */

export enum StatusCondition {
    Normal,
    Burn,
    Freeze,
    Paralysis,
    Poison,
    Toxic,
    Sleep
}

// TODO: this transpiles incorrectly
const statusConditionFlags = {
    [StatusCondition.Burn]: 'Burn',
    [StatusCondition.Freeze]: 'Freeze',
    [StatusCondition.Paralysis]: 'Paralysis',
    [StatusCondition.Poison]: 'Poison',
    [StatusCondition.Toxic]: 'Toxic',
    [StatusCondition.Sleep]: 'Sleep'
};

const statusConditionList: (keyof typeof statusConditionFlags)[] = [
    StatusCondition.Burn,
    StatusCondition.Freeze,
    StatusCondition.Paralysis,
    StatusCondition.Poison,
    StatusCondition.Toxic,
    StatusCondition.Sleep,
];

const numStatusConditions = 6;

export function addStatusCondition(
    condition: StatusCondition,
    fixedSleepDuration?: number
): boolean {
    // TODO: some pokémon are immune to certain status conditions
    if (condition === StatusCondition.Normal) {
        return true;
    }

    if (condition === StatusCondition.Sleep) {
        external.sleep(target.id, fixedSleepDuration || random(1, 3));
    }

    external.addFlagTarget(statusConditionFlags[condition]);
    external.addStatusCondition(target.id, condition);
    return true;
}

export function removeStatusCondition(entity: Pokemon): void {
    for (let i = 0; i < numStatusConditions; i++) {
        const statusCondition = statusConditionList[i];
        const flagId = statusConditionFlags[statusCondition];
        external.removeFlagTarget(flagId);
    }

    external.removeStatusCondition(entity.id);
}

// Burn
export function Flag_Burn_beforeDamageInflict() {
    if (move.kind == 'Physical') {
        external.multiplyDamage(0.5);
    }
}

export function Flag_Burn_onTurnEnd() {
    external.showText(target.displayName + ' is hurt by its burn!');
    external.fixedDamage(Math.ceil(target.hp / 16));
}


// Freeze
export function Flag_Freeze_beforeMove() {
    external.showText(target.displayName + ' is frozen solid!');
    if (random(1, 100) <= 20) {
        external.showText(target.displayName + ' thawed out!');
        removeStatusCondition(target);
    } else {
        external.negateMove();
    }
}


// Paralysis
export function Flag_Paralysis_beforeMove() {
    if (random(1, 100) <= 25) {
        external.showText(target.displayName + ' is paralyzed! It can\'t move!');
        external.negateMove();
    }
}


// Poison
export function Flag_Poison_onTurnEnd() {
    external.showText(target.displayName + ' is hurt by poison!');
    external.fixedDamage(Math.ceil(target.hp / 8));
}


// Toxic
let toxicCounter: { [pokemonId: number]: number } = {};
export function Flag_Toxic_onTurnEnd() {
    external.showText(target.displayName + ' is hurt by poison!');

    if (toxicCounter[target.id] === undefined) {
        toxicCounter[target.id] = 0;
    }

    toxicCounter[target.id]++;
    external.fixedDamage(Math.ceil(toxicCounter[target.id] * target.hp / 8));
}

export function Flag_Toxic_onSwitchIn() {
    toxicCounter[target.id] = 0;
}

export function Flag_Toxic_onBattleEnd() {
    external.removeFlagTarget('Toxic');
    external.addFlagTarget('Poison');
}


// Sleep
export function Flag_Sleep_beforeMove() {
    external.showText(target.displayName + ' is fast asleep.');
    if (target.asleepRounds === 0) {
        external.showText(target.displayName + ' woke up!');
        removeStatusCondition(target);
    } else {
        external.negateMove();
    }
}

export function Flag_Sleep_onTurnEnd() {
    external.reduceSleepCounter();
}
