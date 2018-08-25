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

user = {}
target = {}

function Struggle_onUse()
    damageWithFixedRecoil((user.hp + 3) / 4)
end

function TailWhip_onUse()
    lowerStat(stats.defense, 1)
end
