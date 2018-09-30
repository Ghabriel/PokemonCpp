local typeTable = {{1, 1, 1, 1, 1, 0.5, 1, 0, 0.5, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {2, 1, 0.5, 0.5, 1, 2, 0.5, 0, 2, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5}, {1, 2, 1, 1, 1, 0.5, 2, 1, 0.5, 1, 1, 2, 0.5, 1, 1, 1, 1, 1}, {1, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 0, 1, 1, 2, 1, 1, 1, 1, 1, 2}, {1, 1, 0, 2, 1, 2, 0.5, 1, 2, 2, 1, 0.5, 2, 1, 1, 1, 1, 1}, {1, 0.5, 2, 1, 0.5, 1, 2, 1, 0.5, 2, 1, 1, 1, 1, 2, 1, 1, 1}, {1, 0.5, 0.5, 0.5, 1, 1, 1, 0.5, 0.5, 0.5, 1, 2, 1, 2, 1, 1, 2, 0.5}, {0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 1}, {1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 1, 2, 1, 1, 2}, {1, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5, 0.5, 2, 1, 1, 2, 0.5, 1, 1}, {1, 1, 1, 1, 2, 2, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 1, 0.5, 1, 1}, {1, 1, 0.5, 0.5, 2, 2, 0.5, 1, 0.5, 0.5, 2, 0.5, 1, 1, 1, 0.5, 1, 1}, {1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 0.5, 1, 1}, {1, 2, 1, 2, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 0.5, 1, 1, 0, 1}, {1, 1, 2, 1, 2, 1, 1, 1, 0.5, 0.5, 0.5, 2, 1, 1, 0.5, 2, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 1, 1, 2, 1, 0}, {1, 0.5, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5}, {1, 2, 1, 0.5, 1, 1, 1, 1, 0.5, 0.5, 1, 1, 1, 1, 1, 2, 2, 1}}
local typeMapping = {
    ["Normal"] = 1,
    ["Fighting"] = 2,
    ["Flying"] = 3,
    ["Poison"] = 4,
    ["Ground"] = 5,
    ["Rock"] = 6,
    ["Bug"] = 7,
    ["Ghost"] = 8,
    ["Steel"] = 9,
    ["Fire"] = 10,
    ["Water"] = 11,
    ["Grass"] = 12,
    ["Electric"] = 13,
    ["Psychic"] = 14,
    ["Ice"] = 15,
    ["Dragon"] = 16,
    ["Dark"] = 17,
    ["Fairy"] = 18
}
StatFlag = {
    All = 0,
    IgnorePositive = 1,
    IgnoreNegative = 2,
}
Stat = {
    HP = 0,
    Attack = 1,
    Defense = 2,
    SpecialAttack = 3,
    SpecialDefense = 4,
    Speed = 5,
    Accuracy = 6,
    Evasion = 7,
}
function getStatStageMultiplier(stage)
    local absStage = Math.abs(stage)
    if stage >= 0 then
        return (2 + absStage) / 2
    else
        return 2 / (2 + absStage)
    end
end
function getAccuracyStatStageMultiplier(stage)
    local absStage = Math.abs(stage)
    if stage >= 0 then
        return (3 + absStage) / 3
    else
        return 3 / (3 + absStage)
    end
end
function getAttackStatForMove(pokemon, move, calculationFlags)
    if move.kind === "Physical" then
        return getEffectiveStat(pokemon, Stat.Attack, calculationFlags)
    end
    if move.kind === "Special" then
        return getEffectiveStat(pokemon, Stat.SpecialAttack, calculationFlags)
    end
    return 0
end
function getDefenseStatForMove(pokemon, move, calculationFlags)
    if move.kind === "Physical" then
        return getEffectiveStat(pokemon, Stat.Defense, calculationFlags)
    end
    if move.kind === "Special" then
        return getEffectiveStat(pokemon, Stat.SpecialDefense, calculationFlags)
    end
    return 0
end
function getEffectiveStat(pokemon, stat, calculationFlags)
    local standardStatValue = getStandardStat(pokemon, stat)
    local currentStage = getModifiedStatStage(pokemon, stat, calculationFlags)
    local statValue = standardStatValue * getStatStageMultiplier(currentStage)
    return statValue
end
function getStandardStat(pokemon, stat)
    return external.getStandardStat(pokemon.id, stat)
end
function getModifiedStatStage(pokemon, stat, calculationFlags)
    local currentStage = getStatStage(pokemon, stat)
    if calculationFlags === StatFlag.IgnorePositive then
        currentStage = Math.min(0, currentStage)
    else
        if calculationFlags === StatFlag.IgnoreNegative then
            currentStage = Math.max(0, currentStage)
        end
    end
    return currentStage
end
function getStatStage(pokemon, stat)
    return external.getStatStage(pokemon.id, stat)
end
function hasUsableMoves(pokemon)
    return true
end
function getPP(pokemon, moveIndex)
    return 42
end
function checkMiss(user, target, move)
    if move.accuracy === 0 then
        return false
    end
    local userAccuracyStage = getStatStage(user, Stat.Accuracy)
    local targetEvasionStage = getStatStage(target, Stat.Evasion)
    local accuracyStage = clamp(userAccuracyStage - targetEvasionStage, -6, 6)
    local accuracyStageMultiplier = getAccuracyStatStageMultiplier(accuracyStage)
    local hitRate = move.accuracy * accuracyStageMultiplier
    external.log("Hit rate: " + hitRate)
    return random(1, 100) > hitRate
end
function clamp(value, minValue, maxValue)
    return Math.max(minValue, Math.min(maxValue, value))
end
function checkCritical(user, target, move)
    local criticalHitStage = getCriticalHitStage(user)
    local chancesIn24
    if criticalHitStage === 0 then
        chancesIn24 = 1
    else
        if criticalHitStage === 1 then
            chancesIn24 = 3
        else
            if criticalHitStage === 2 then
                chancesIn24 = 12
            else
                chancesIn24 = 24
            end
        end
    end
    return random(1, 24) <= chancesIn24
end
function getCriticalHitStage(pokemon)
    return 0
end
function random(min, max)
    return external.random(min, max)
end
function getTypeEffectiveness(pokemon, move)
    if move.type === "???" then
        return 1
    end
    local moveTypeIndex = typeMapping[move.type]
    local firstTypeIndex = typeMapping[pokemon.type0]
    local result = typeTable[moveTypeIndex][firstTypeIndex]
    if pokemon.typeCount > 1 then
        local secondTypeIndex = typeMapping[pokemon.type1]
        result = result * typeTable[moveTypeIndex][secondTypeIndex]
    end
    return result
end
