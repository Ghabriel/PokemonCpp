require "resources.scripts.battle-formulas"
require "resources.scripts.battle-utils"
require "resources.scripts.battle-variables"
require "resources.scripts.status-conditions"

function Struggle_onUse()
    damageWithFixedRecoil((user.hp + 3) / 4)
end

function TailWhip_onUse()
    -- lowerStat(stats.defense, 1)
    showText("Applying status condition: Sleep")
    addStatusCondition(statusConditions.sleep, 1)
end
