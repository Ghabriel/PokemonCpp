typeTable = {{1, 1, 1, 1, 1, 0.5, 1, 0, 0.5, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {2, 1, 0.5, 0.5, 1, 2, 0.5, 0, 2, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5}, {1, 2, 1, 1, 1, 0.5, 2, 1, 0.5, 1, 1, 2, 0.5, 1, 1, 1, 1, 1}, {1, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 0, 1, 1, 2, 1, 1, 1, 1, 1, 2}, {1, 1, 0, 2, 1, 2, 0.5, 1, 2, 2, 1, 0.5, 2, 1, 1, 1, 1, 1}, {1, 0.5, 2, 1, 0.5, 1, 2, 1, 0.5, 2, 1, 1, 1, 1, 2, 1, 1, 1}, {1, 0.5, 0.5, 0.5, 1, 1, 1, 0.5, 0.5, 0.5, 1, 2, 1, 2, 1, 1, 2, 0.5}, {0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 1}, {1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 1, 2, 1, 1, 2}, {1, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5, 0.5, 2, 1, 1, 2, 0.5, 1, 1}, {1, 1, 1, 1, 2, 2, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 1, 0.5, 1, 1}, {1, 1, 0.5, 0.5, 2, 2, 0.5, 1, 0.5, 0.5, 2, 0.5, 1, 1, 1, 0.5, 1, 1}, {1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 0.5, 1, 1}, {1, 2, 1, 2, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 0.5, 1, 1, 0, 1}, {1, 1, 2, 1, 2, 1, 1, 1, 0.5, 0.5, 0.5, 2, 1, 1, 0.5, 2, 1, 1}, {1, 1, 1, 1, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 1, 1, 2, 1, 0}, {1, 0.5, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5}, {1, 2, 1, 0.5, 1, 1, 1, 1, 0.5, 0.5, 1, 1, 1, 1, 1, 2, 2, 1}}
typeMapping = {
    ['Normal'] = 1,
    ['Fighting'] = 2,
    ['Flying'] = 3,
    ['Poison'] = 4,
    ['Ground'] = 5,
    ['Rock'] = 6,
    ['Bug'] = 7,
    ['Ghost'] = 8,
    ['Steel'] = 9,
    ['Fire'] = 10,
    ['Water'] = 11,
    ['Grass'] = 12,
    ['Electric'] = 13,
    ['Psychic'] = 14,
    ['Ice'] = 15,
    ['Dragon'] = 16,
    ['Dark'] = 17,
    ['Fairy'] = 18
}
function getStatStageMultiplier(stage)
    local absStage = math.abs(stage)
    if stage >= 0 then
        return (2 + absStage) / 2
    else
        return 2 / (2 + absStage)
    end
end
function getAccuracyStatStageMultiplier(stage)
    local absStage = math.abs(stage)
    if stage >= 0 then
        return (3 + absStage) / 3
    else
        return 3 / (3 + absStage)
    end
end
function getAttackStatForMove(pokemon, move, calculationFlags)
    setmetatable(pokemon, pokemonMetatable)
    setmetatable(move, moveMetatable)
    if move.kind == "Physical" then
        return getEffectiveStat(pokemon, Stat.Attack, calculationFlags)
    end
    if move.kind == "Special" then
        return getEffectiveStat(pokemon, Stat.SpecialAttack, calculationFlags)
    end
    return 0
end
function getDefenseStatForMove(pokemon, move, calculationFlags)
    setmetatable(pokemon, pokemonMetatable)
    setmetatable(move, moveMetatable)
    if move.kind == "Physical" then
        return getEffectiveStat(pokemon, Stat.Defense, calculationFlags)
    end
    if move.kind == "Special" then
        return getEffectiveStat(pokemon, Stat.SpecialDefense, calculationFlags)
    end
    return 0
end
function getEffectiveStat(pokemon, stat, calculationFlags)
    setmetatable(pokemon, pokemonMetatable)
    local standardStatValue = getStandardStat(pokemon, stat)
    local currentStage = getModifiedStatStage(pokemon, stat, calculationFlags)
    local statValue = standardStatValue * getStatStageMultiplier(currentStage)
    return specialized:getEffectiveStat(pokemon, stat, statValue)
end
function getStandardStat(pokemon, stat)
    setmetatable(pokemon, pokemonMetatable)
    return external.getStandardStat(pokemon.id, stat)
end
function getModifiedStatStage(pokemon, stat, calculationFlags)
    setmetatable(pokemon, pokemonMetatable)
    local currentStage = getStatStage(pokemon, stat)
    if calculationFlags == StatFlag.IgnorePositive then
        currentStage = math.min(0, currentStage)
    else
        if calculationFlags == StatFlag.IgnoreNegative then
            currentStage = math.max(0, currentStage)
        end
    end
    return currentStage
end
function getStatStage(pokemon, stat)
    setmetatable(pokemon, pokemonMetatable)
    return external.getStatStage(pokemon.id, stat)
end
function hasUsableMoves(pokemon)
    setmetatable(pokemon, pokemonMetatable)
    for i = 0, pokemon.moveCount - 1 do
        if canUseMove(pokemon, i) then
            return true
        end
    end
    return false
end
function canUseMove(pokemon, moveIndex)
    setmetatable(pokemon, pokemonMetatable)
    return getPP(pokemon, moveIndex) > 0 and specialized:canUseMove(pokemon, moveIndex)
end
function getPP(pokemon, moveIndex)
    setmetatable(pokemon, pokemonMetatable)
    if moveIndex == 0 then
        return pokemon.pp0
    elseif moveIndex == 1 then
        return pokemon.pp1
    elseif moveIndex == 2 then
        return pokemon.pp2
    elseif moveIndex == 3 then
        return pokemon.pp3
    end
    return 0
end
function checkMiss(user, target, move)
    setmetatable(user, pokemonMetatable)
    setmetatable(target, pokemonMetatable)
    setmetatable(move, moveMetatable)
    if move.accuracy == 0 then
        return false
    end
    local userAccuracyStage = getStatStage(user, Stat.Accuracy)
    local targetEvasionStage = getStatStage(target, Stat.Evasion)
    local accuracyStage = clamp(userAccuracyStage - targetEvasionStage, -6, 6)
    local accuracyStageMultiplier = getAccuracyStatStageMultiplier(accuracyStage)
    local hitRate = move.accuracy * accuracyStageMultiplier
    external.log("Hit rate: " .. hitRate)
    return random(1, 100) > hitRate
end
function clamp(value, minValue, maxValue)
    return math.max(minValue, math.min(maxValue, value))
end
function checkCritical(user, target, move)
    setmetatable(user, pokemonMetatable)
    setmetatable(target, pokemonMetatable)
    setmetatable(move, moveMetatable)
    local criticalHitStage = getCriticalHitStage(user)
    local chancesIn24
    if criticalHitStage == 0 then
        chancesIn24 = 1
    elseif criticalHitStage == 1 then
        chancesIn24 = 3
    elseif criticalHitStage == 2 then
        chancesIn24 = 12
    else
        chancesIn24 = 24
    end
    return random(1, 24) <= chancesIn24
end
function getCriticalHitStage(pokemon)
    setmetatable(pokemon, pokemonMetatable)
    return 0
end
function random(min, max)
    return external.random(min, max)
end
function getTypeEffectiveness(pokemon, move)
    setmetatable(pokemon, pokemonMetatable)
    setmetatable(move, moveMetatable)
    if move.type == "???" then
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
function calculateExpGain(winner, fainted)
    setmetatable(winner, pokemonMetatable)
    setmetatable(fainted, pokemonMetatable)
    local luckyEgg = 1
    local shareFactor = 1
    local factor1 = (fainted.baseExp * fainted.level) / (5 * shareFactor)
    local factor2 = math.pow(2 * fainted.level + 10, 2.5)
    local factor3 = math.pow(fainted.level + winner.level + 10, 2.5)
    return (factor1 * (factor2 / factor3) + 1) * luckyEgg
end
