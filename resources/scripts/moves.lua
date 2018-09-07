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

playerTeam = {}
opponentTeam = {}
user = {}
target = {}
move = {}

function Struggle_onUse()
    damageWithFixedRecoil((user.hp + 3) / 4)
end

function TailWhip_onUse()
    -- lowerStat(stats.defense, 1)
    showText("It's... FREEZING TIME!!")
    addFlag("Freeze")
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

function Flag_Paralysis_onTurnStart()
    multiplyStat(stats.speed, 0.5)
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
    toxicCounter[target] = toxicCounter[target] + 1
    fixedDamage((toxicCounter * target.hp + 7) / 8)
end

function Flag_Toxic_onSwitchIn()
    toxicCounter[target] = 0
end

function Flag_Toxic_onBattleEnd()
    removeFlag("Toxic")
    addFlag("Poison")
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
