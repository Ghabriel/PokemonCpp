typeTable = {
    {1, 1, 1, 1, 1, 0.5, 1, 0, 0.5, 1, 1, 1, 1, 1, 1, 1, 1, 1},
    {2, 1, 0.5, 0.5, 1, 2, 0.5, 0, 2, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5},
    {1, 2, 1, 1, 1, 0.5, 2, 1, 0.5, 1, 1, 2, 0.5, 1, 1, 1, 1, 1},
    {1, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 0, 1, 1, 2, 1, 1, 1, 1, 1, 2},
    {1, 1, 0, 2, 1, 2, 0.5, 1, 2, 2, 1, 0.5, 2, 1, 1, 1, 1, 1},
    {1, 0.5, 2, 1, 0.5, 1, 2, 1, 0.5, 2, 1, 1, 1, 1, 2, 1, 1, 1},
    {1, 0.5, 0.5, 0.5, 1, 1, 1, 0.5, 0.5, 0.5, 1, 2, 1, 2, 1, 1, 2, 0.5},
    {0, 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 1},
    {1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5, 0.5, 1, 0.5, 1, 2, 1, 1, 2},
    {1, 1, 1, 1, 1, 0.5, 2, 1, 2, 0.5, 0.5, 2, 1, 1, 2, 0.5, 1, 1},
    {1, 1, 1, 1, 2, 2, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 1, 0.5, 1, 1},
    {1, 1, 0.5, 0.5, 2, 2, 0.5, 1, 0.5, 0.5, 2, 0.5, 1, 1, 1, 0.5, 1, 1},
    {1, 1, 2, 1, 0, 1, 1, 1, 1, 1, 2, 0.5, 0.5, 1, 1, 0.5, 1, 1},
    {1, 2, 1, 2, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 0.5, 1, 1, 0, 1},
    {1, 1, 2, 1, 2, 1, 1, 1, 0.5, 0.5, 0.5, 2, 1, 1, 0.5, 2, 1, 1},
    {1, 1, 1, 1, 1, 1, 1, 1, 0.5, 1, 1, 1, 1, 1, 1, 2, 1, 0},
    {1, 0.5, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 2, 1, 1, 0.5, 0.5},
    {1, 2, 1, 0.5, 1, 1, 1, 1, 0.5, 0.5, 1, 1, 1, 1, 1, 2, 2, 1},
}

typeMapping = {
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
    ["Fairy"] = 18,
}

statFlags = {
    all = 0,
    ignorePositive = 1,
    ignoreNegative = 2
}

-- float(int)
function getStatStageMultiplier(stage)
    local absStage = math.abs(stage);
    if stage >= 0 then
        return (2 + absStage) / 2.0
    else
        return 2.0 / (2 + absStage)
    end
end

-- float(int)
function getAccuracyStatStageMultiplier(stage)
    local absStage = math.abs(stage);
    if stage >= 0 then
        return (3 + absStage) / 3.0
    else
        return 3.0 / (3 + absStage)
    end
end

-- int(Pokemon, Move, StatFlags)
function getAttackStatForMove(pokemon, move, calculationFlags)
    if move.kind == "Physical" then
        return getEffectiveStat(pokemon, stats.attack, calculationFlags)
    end

    if move.kind == "Special" then
        return getEffectiveStat(pokemon, stats.specialAttack, calculationFlags)
    end

    return 0
end

-- int(Pokemon, Move, StatFlags)
function getDefenseStatForMove(pokemon, move, calculationFlags)
    if move.kind == "Physical" then
        return getEffectiveStat(pokemon, stats.defense, calculationFlags)
    end

    if move.kind == "Special" then
        return getEffectiveStat(pokemon, stats.specialDefense, calculationFlags)
    end

    return 0
end

-- int(Pokemon, Stat, StatFlags)
function getEffectiveStat(pokemon, stat, calculationFlags)
    local standardStatValue = external.getStandardStat(pokemon, stat)
    local currentStage = getModifiedStatStage(pokemon, stat, calculationFlags)
    local statValue = standardStatValue * getStatStageMultiplier(currentStage)
    return specialized.getEffectiveStat(pokemon, stat, statValue)
end

-- int(Pokemon, Stat, StatFlags)
function getModifiedStatStage(pokemon, stat, calculationFlags)
    local currentStage = external.getStatStage(pokemon, stat)

    if calculationFlags == statFlags.ignorePositive then
        currentStage = math.min(0, currentStage)
    elseif calculationFlags == statFlags.ignoreNegative then
        currentStage = math.max(0, currentStage)
    end

    return currentStage
end

-- bool(Pokemon)
function hasUsableMoves(pokemon)
    for moveIndex = 0,(pokemon.moveCount - 1) do
        if canUseMove(pokemon, moveIndex) then
            return true
        end
    end

    return false
end

-- bool(Pokemon, int)
function canUseMove(pokemon, moveIndex)
    return getPP(pokemon, moveIndex) > 0 and specialized.canUseMove(pokemon, moveIndex)
end

-- int(Pokemon, int)
function getPP(pokemon, moveIndex)
    if     moveIndex == 0 then return pokemon.pp0
    elseif moveIndex == 1 then return pokemon.pp1
    elseif moveIndex == 2 then return pokemon.pp2
    elseif moveIndex == 3 then return pokemon.pp3
    end

    return 0
end

-- bool(Pokemon, Pokemon, Move)
function checkMiss(user, target, move)
    if move.accuracy == 0 then
        return false
    end

    local userAccuracyStage = external.getStatStage(user, stats.accuracy)
    local targetEvasionStage = external.getStatStage(target, stats.evasion)
    local accuracyStage = clamp(userAccuracyStage - targetEvasionStage, -6, 6)
    local accuracyStageMultiplier = getAccuracyStatStageMultiplier(accuracyStage)
    local hitRate = move.accuracy * accuracyStageMultiplier
    log("Hit rate: "..hitRate)
    return random(1, 100) > hitRate
end

-- int(int, int, int)
function clamp(value, minValue, maxValue)
    return math.max(minValue, math.min(maxValue, value))
end

-- bool(Pokemon, Pokemon, Move)
function checkCritical(user, target, move)
    local criticalHitStage = getCriticalHitStage(user)
    local chancesIn24;

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

-- int(int, int)
function random(min, max)
    return external.random(min, max)
end

-- float(Pokemon, Move)
function getTypeEffectiveness(pokemon, move)
    -- Struggle and other typeless moves
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

-- int(Pokemon, Pokemon)
function calculateExpGain(winner, fainted)
    local luckyEgg = 1 -- TODO: handle Lucky Egg
    local shareFactor = 1 -- TODO: handle Exp. Share
    local factor1 = (fainted.baseExp * fainted.level) / (5 * shareFactor)
    local factor2 = math.pow(2 * fainted.level + 10, 2.5)
    local factor3 = math.pow(fainted.level + winner.level + 10, 2.5)
    return (factor1 * (factor2 / factor3) + 1) * luckyEgg
end
