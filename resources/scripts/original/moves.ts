import { user } from './battle-variables';
import { addStatusCondition, StatusCondition } from './status-conditions';
import { external, luaImport } from './types';

luaImport('resources.scripts.enums');
luaImport('resources.scripts.battle-formulas');
luaImport('resources.scripts.battle-utils');
luaImport('resources.scripts.battle-variables');
luaImport('resources.scripts.status-conditions');

export function Struggle_onUse() {
    external.damageWithFixedRecoil(Math.ceil(user.hp / 4));
}

export function TailWhip_onUse() {
    // lowerStat(Stat.Defense, 1);
    external.showText('Applying status condition: Sleep');
    addStatusCondition(StatusCondition.Sleep, 1);
}
