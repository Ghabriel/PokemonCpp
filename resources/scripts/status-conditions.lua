--[[
    Burn - 90% (not checking faint)
    Freeze - 100%
    Paralysis - 100%
    Poison - 90% (not checking faint)
    Toxic - 80% (onSwitchIn and onBattleEnd are untestable at the moment)
    Sleep - 100%
]]

statusConditions = {
    burn = 1,
    freeze = 2,
    paralysis = 3,
    poison = 4,
    toxic = 5,
    sleep = 6
}

statusConditionFlags = {
    [statusConditions.burn] = "Burn",
    [statusConditions.freeze] = "Freeze",
    [statusConditions.paralysis] = "Paralysis",
    [statusConditions.poison] = "Poison",
    [statusConditions.toxic] = "Toxic",
    [statusConditions.sleep] = "Sleep"
}

function addStatusCondition(conditionId, fixedSleepDuration)
    -- TODO: some pokémon are immune to certain status conditions
    if conditionId == statusConditions.sleep then
        external.sleep(target.id, fixedSleepDuration or random(1, 3))
    end

    external.addFlagTarget(statusConditionFlags[conditionId])
    external.addStatusCondition(target.id, conditionId)
    return true
end

function removeStatusCondition(entity)
    for condition, flagId in pairs(statusConditionFlags) do
        external.removeFlagTarget(flagId)
    end

    external.removeStatusCondition(entity.id)
end

-- Burn
function Flag_Burn_beforeDamageInflict()
    if move.kind == "Physical" then
        external.multiplyDamage(0.5)
    end
end

function Flag_Burn_onTurnEnd()
    external.showText(target.displayName.." is hurt by its burn!")
    external.fixedDamage((target.hp + 15) / 16)
end


-- Freeze
function Flag_Freeze_beforeMove()
    external.showText(target.displayName.." is frozen solid!")
    if random(1, 100) <= 20 then
        external.showText(target.displayName.." thawed out!")
        external.removeStatusCondition(target)
    else
        external.negateMove()
    end
end


-- Paralysis
function Flag_Paralysis_beforeMove()
    if random(1, 100) <= 25 then
        external.showText(target.displayName.." is paralyzed! It can't move!")
        external.negateMove()
    end
end


-- Poison
function Flag_Poison_onTurnEnd()
    external.showText(target.displayName.." is hurt by poison!")
    external.fixedDamage((target.hp + 7) / 8)
end


-- Toxic
toxicCounter = {}
function Flag_Toxic_onTurnEnd()
    external.showText(target.displayName.." is hurt by poison!")

    if toxicCounter[target] == nil then
        toxicCounter[target] = 0
    end

    toxicCounter[target] = toxicCounter[target] + 1
    external.fixedDamage((toxicCounter[target] * target.hp + 7) / 8)
end

function Flag_Toxic_onSwitchIn()
    toxicCounter[target] = 0
end

function Flag_Toxic_onBattleEnd()
    external.removeFlagTarget("Toxic")
    external.addFlagTarget("Poison")
end


-- Sleep
function Flag_Sleep_beforeMove()
    external.showText(target.displayName.." is fast asleep.")
    if tonumber(target.asleepRounds) == 0 then
        external.showText(target.displayName.." woke up!")
        removeStatusCondition(target)
    else
        external.negateMove()
    end
end

function Flag_Sleep_onTurnEnd()
    external.reduceSleepCounter()
end
