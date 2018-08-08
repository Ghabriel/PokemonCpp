function interact(x, y)
    if x == 2 and y == 1 then
        write("Taking control...")
        disableControls()
        local delay = 50
        for i = 0,5 do
            turnPlayerSouth()
            wait(delay)
            turnPlayerEast()
            wait(delay)
            turnPlayerNorth()
            wait(delay)
            turnPlayerWest()
            wait(delay)
        end
        -- movePlayerSouth(5)
        -- movePlayerEast(5)
        -- wait(1000)
        -- movePlayerNorth(5)
        -- movePlayerWest(5)
        enableControls()
    end
end

function isTileBlocked(x, y)
    return x < 0 or x >= 30
        or y < 0 or y >= 25
        or (x == 2 and y == 1)
end
