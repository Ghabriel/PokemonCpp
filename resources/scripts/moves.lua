stats = {
    hp = 0,
    attack = 1,
    defense = 2,
    specialAttack = 3,
    specialDefense = 4,
    speed = 5,
    accuracy = 6,
    evasion = 7
}

user = { id = -1 }
target = { id = -2 }
userTeam = {
    [0] = { id = -3 },
    [1] = { id = -4 },
    [2] = { id = -5 },
    [3] = { id = -6 },
    [4] = { id = -7 },
}
targetTeam = {
    [0] = { id = -8 },
    [1] = { id = -9 },
    [2] = { id = -10 },
    [3] = { id = -11 },
    [4] = { id = -12 },
}
move = {}

function getTargetEffectiveStat(stat, baseValue)
    if stat == stats.speed and hasFlag(target.id, "Paralysis") then
        baseValue = baseValue * 0.5
    end

    return baseValue
end

function Struggle_onUse()
    damageWithFixedRecoil((user.hp + 3) / 4)
end

function TailWhip_onUse()
    -- lowerStat(stats.defense, 1)
    showText("It's... PARALYSIS TIME!!")
    addFlagTarget("Paralysis")
    -- if not hasType("Fire") then
    --     addFlag("Burn")
    -- end
end

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
