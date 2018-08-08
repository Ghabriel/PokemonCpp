function interact(x, y)
    if x == 2 and y == 1 then
        write("Taking control...")
        disableControls()
        movePlayerSouth(5)
        movePlayerEast(5)
        wait(1000)
        movePlayerNorth(5)
        movePlayerWest(5)
        enableControls()
    end
end

function isTileBlocked(x, y)
    return x < 0 or x >= 30
        or y < 0 or y >= 25
        or (x == 2 and y == 1)
end
