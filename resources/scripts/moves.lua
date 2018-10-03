require "resources.scripts.battle-formulas"
require "resources.scripts.battle-utils"
require "resources.scripts.battle-variables"
require "resources.scripts.status-conditions"
function Struggle_onUse()
    external.damageWithFixedRecoil(math.ceil(user.hp / 4))
end
function TailWhip_onUse()
    external.showText("Applying status condition: Sleep")
    addStatusCondition(StatusCondition.Sleep, 1)
end
