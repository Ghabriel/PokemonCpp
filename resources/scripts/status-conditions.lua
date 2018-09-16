--[[
    Burn - 85% (not reducing damage, not checking faint)
    Freeze - 100%
    Paralysis - 100%
    Poison- 90% (not checking faint)
    Toxic - 80% (onSwitchIn and onBattleEnd are untestable at the moment)
    Sleep - 0%
]]

-- Burn
function Flag_Burn_beforeDamageInflict()
    if move.kind == "Physical" then
        multiplyDamage(0.5)
    end
end

function Flag_Burn_onTurnEnd()
    showText(target.displayName.." is hurt by its burn!")
    fixedDamage((target.hp + 15) / 16)
end


-- Freeze
function Flag_Freeze_beforeMove()
    showText(target.displayName.." is frozen solid!")
    if random(1, 100) <= 20 then
        showText(target.displayName.." thawed out!")
        removeFlagTarget("Freeze")
    else
        negateMove()
    end
end


-- Paralysis
function Flag_Paralysis_beforeMove()
    if random(1, 100) <= 25 then
        showText(target.displayName.." is paralyzed! It can't move!")
        negateMove()
    end
end


-- Poison
function Flag_Poison_onTurnEnd()
    showText(target.displayName.." is hurt by poison!")
    fixedDamage((target.hp + 7) / 8)
end


-- Toxic
toxicCounter = {}
function Flag_Toxic_onTurnEnd()
    showText(target.displayName.." is hurt by poison!")

    if toxicCounter[target] == nil then
        toxicCounter[target] = 0
    end

    toxicCounter[target] = toxicCounter[target] + 1
    fixedDamage((toxicCounter[target] * target.hp + 7) / 8)
end

function Flag_Toxic_onSwitchIn()
    toxicCounter[target] = 0
end

function Flag_Toxic_onBattleEnd()
    removeFlagTarget("Toxic")
    addFlagTarget("Poison")
end


-- Sleep
function Flag_Sleep_beforeMove()
    showText(target.displayName.." is fast asleep.")
    if target.asleepRounds == 0 then
        showText(target.displayName.." woke up!")
    else
        negateMove()
    end
end
