import { user } from './battle-variables';
import { addStatusCondition, StatusCondition } from './status-conditions';
import { external } from './types';

// require 'resources.scripts.battle-formulas'
// require 'resources.scripts.battle-utils'
// require 'resources.scripts.battle-variables'
// require 'resources.scripts.status-conditions'

export function Struggle_onUse() {
    external.damageWithFixedRecoil(Math.ceil(user.hp / 4));
}

export function TailWhip_onUse() {
    // lowerStat(Stat.Defense, 1);
    external.showText('Applying status condition: Sleep');
    addStatusCondition(StatusCondition.Sleep, 1);
}
