function Struggle_onUse()
    external.damageWithFixedRecoil(math.ceil(user.hp / 4))
end
function TailWhip_onUse()
    external.showText("Applying status condition: Sleep")
    addStatusCondition(StatusCondition.Sleep, 1)
end
