function interact(x, y)
    if x == 2 and y == 1 then
        showText("I'm a rock.");
        showText("Never forget that.");
    end
end

function isTileBlocked(x, y)
    return x < 0 or x >= 30
        or y < 0 or y >= 25
        or (x == 2 and y == 1)
end
