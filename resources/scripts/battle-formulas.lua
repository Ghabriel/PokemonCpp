function getTargetEffectiveStat(stat, baseValue)
    if stat == stats.speed and hasFlag(target.id, "Paralysis") then
        baseValue = baseValue * 0.5
    end

    return baseValue
end
