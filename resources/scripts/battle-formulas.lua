specialized = {}

function specialized:getEffectiveStat(pokemon, stat, baseValue)
    if stat == stats.speed and external.hasFlag(pokemon.id, "Paralysis") then
        baseValue = baseValue * 0.5
    end

    return baseValue
end

--[[
    Given the index of a move that has enough PP, checks if it can be used. Moves
    like Disable, items like Choice Band, etc should be considered.
    Signature: bool(Pokemon, int)
]]
function specialized:canUseMove(pokemon, moveIndex)
    -- TODO
    return true
end
