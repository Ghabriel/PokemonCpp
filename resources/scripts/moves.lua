require "resources.scripts.battle-formulas"
require "resources.scripts.status-conditions"

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
