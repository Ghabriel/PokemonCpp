function interact(x, y)
    if x == 2 and y == 1 then
        write("Taking control...")
        movePlayerSouth(5)
    end
end

function isTileBlocked(x, y)
    return x < 0 or x >= 30
        or y < 0 or y >= 25
        or (x == 2 and y == 1)
end