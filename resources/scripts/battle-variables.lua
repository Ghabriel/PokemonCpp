stats = {
    hp = 0,
    attack = 1,
    defense = 2,
    specialAttack = 3,
    specialDefense = 4,
    speed = 5,
    accuracy = 6,
    evasion = 7
}

defaultEntityIds = {
    user = -1,
    target = -2,
    userTeam = {
        [0] = -3,
        [1] = -4,
        [2] = -5,
        [3] = -6,
        [4] = -7,
    },
    targetTeam = {
        [0] = -8,
        [1] = -9,
        [2] = -10,
        [3] = -11,
        [4] = -12,
    },
    move = -13
}

user = { id = defaultEntityIds.user }
target = { id = defaultEntityIds.target }
userTeam = {
    [0] = { id = defaultEntityIds.userTeam[0] },
    [1] = { id = defaultEntityIds.userTeam[1] },
    [2] = { id = defaultEntityIds.userTeam[2] },
    [3] = { id = defaultEntityIds.userTeam[3] },
    [4] = { id = defaultEntityIds.userTeam[4] },
}
targetTeam = {
    [0] = { id = defaultEntityIds.targetTeam[0] },
    [1] = { id = defaultEntityIds.targetTeam[1] },
    [2] = { id = defaultEntityIds.targetTeam[2] },
    [3] = { id = defaultEntityIds.targetTeam[3] },
    [4] = { id = defaultEntityIds.targetTeam[4] },
}
move = { pokemonId = defaultEntityIds.move }
