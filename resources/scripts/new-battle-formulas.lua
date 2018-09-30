specialized = {}
function specialized:getEffectiveStat(pokemon, stat, baseValue)
    if stat == Stat.Speed and external.hasFlag(pokemon.id, "Paralysis") then
        baseValue = baseValue * 0.5
    end
    return baseValue
end
function specialized:canUseMove(pokemon, moveIndex)
    return true
end
