function interact(x, y)
    if x == 1 and y == 1 then
        showText("Items cannot be picked up yet.")
    elseif x == 2 and y == 1 then
        showText("Trick Room reverses the moveorder within each priority bracket so that Pokemon with a lower Speed stat attack first, while those with a higher Speed stat will attack last. Individual brackets are still maintained; moves in higher priority brackets still work before moves in lower ones regardless of Trick Room. This effect lasts for five turns, and using Trick Room counts as the first turn. Similar to Magic Room and Wonder Room, using Trick Room while it is already in effect will end it immediately.");
        showText("And that's all for today. Oh, and never forget:")
        showText("I'm a rock.")
    end
end

function isTileBlocked(x, y)
    return x < 0 or x >= 30
        or y < 0 or y >= 25
        or (x == 1 and y == 1)
        or (x == 2 and y == 1)
        or (x == 9 and y <= 2)
        or (x >= 10 and x <= 13 and y <= 2)
end

function onTileStep(x, y)
    if x == 1 and y >= 4 and y <= 6 then
        possibleWildBattle()
        return true
    end

    return false
end
