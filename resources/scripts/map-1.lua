function interact(x, y)
    if x == 2 and y == 1 then
        showText("Trick Room reverses the move order within each priority bracket so that Pokémon with a lower Speed stat attack first, while those with a higher Speed stat will attack last. Individual brackets are still maintained; moves in higher priority brackets still work before moves in lower ones regardless of Trick Room. This effect lasts for five turns, and using Trick Room counts as the first turn. Similar to Magic Room and Wonder Room, using Trick Room while it is already in effect will end it immediately.");
    end
end

function isTileBlocked(x, y)
    return x < 0 or x >= 30
        or y < 0 or y >= 25
        or (x == 2 and y == 1)
end
