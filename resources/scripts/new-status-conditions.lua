StatusCondition = {
    Normal = 0,
    Burn = 1,
    Freeze = 2,
    Paralysis = 3,
    Poison = 4,
    Toxic = 5,
    Sleep = 6,
}
statusConditionFlags = {
    [StatusCondition.Burn] = "Burn",
    [StatusCondition.Freeze] = "Freeze",
    [StatusCondition.Paralysis] = "Paralysis",
    [StatusCondition.Poison] = "Poison",
    [StatusCondition.Toxic] = "Toxic",
    [StatusCondition.Sleep] = "Sleep"
}
statusConditionList = {StatusCondition.Burn, StatusCondition.Freeze, StatusCondition.Paralysis, StatusCondition.Poison, StatusCondition.Toxic, StatusCondition.Sleep}
numStatusConditions = 6
function addStatusCondition(condition, fixedSleepDuration)
    if condition == StatusCondition.Normal then
        return true
    end
    if condition == StatusCondition.Sleep then
        external.sleep(target.id, fixedSleepDuration or random(1, 3))
    end
    external.addFlagTarget(statusConditionFlags[condition])
    external.addStatusCondition(target.id, condition)
    return true
end
function removeStatusCondition(entity)
    for i = 0, numStatusConditions - 1 do
        local statusCondition = statusConditionList[i]
        local flagId = statusConditionFlags[statusCondition]
        external.removeFlagTarget(flagId)
    end
    external.removeStatusCondition(entity.id)
end
function Flag_Burn_beforeDamageInflict()
    if move.kind == "Physical" then
        external.multiplyDamage(0.5)
    end
end
function Flag_Burn_onTurnEnd()
    external.showText(target.displayName .. " is hurt by its burn!")
    external.fixedDamage(math.ceil(target.hp / 16))
end
function Flag_Freeze_beforeMove()
    external.showText(target.displayName .. " is frozen solid!")
    if random(1, 100) <= 20 then
        external.showText(target.displayName .. " thawed out!")
        removeStatusCondition(target)
    else
        external.negateMove()
    end
end
function Flag_Paralysis_beforeMove()
    if random(1, 100) <= 25 then
        external.showText(target.displayName .. " is paralyzed! It can't move!")
        external.negateMove()
    end
end
function Flag_Poison_onTurnEnd()
    external.showText(target.displayName .. " is hurt by poison!")
    external.fixedDamage(math.ceil(target.hp / 8))
end
toxicCounter = {}
function Flag_Toxic_onTurnEnd()
    external.showText(target.displayName .. " is hurt by poison!")
    if toxicCounter[target.id] == nil then
        toxicCounter[target.id] = 0
    end
    toxicCounter[target.id] = toxicCounter[target.id] + 1
    external.fixedDamage(math.ceil(toxicCounter[target.id] * target.hp / 8))
end
function Flag_Toxic_onSwitchIn()
    toxicCounter[target.id] = 0
end
function Flag_Toxic_onBattleEnd()
    external.removeFlagTarget("Toxic")
    external.addFlagTarget("Poison")
end
function Flag_Sleep_beforeMove()
    external.showText(target.displayName .. " is fast asleep.")
    if target.asleepRounds == 0 then
        external.showText(target.displayName .. " woke up!")
        removeStatusCondition(target)
    else
        external.negateMove()
    end
end
function Flag_Sleep_onTurnEnd()
    external.reduceSleepCounter()
end
