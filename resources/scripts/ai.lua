user = {}
foe = {}

function chooseMoveWildBattle()
    log(user.species)
    log(user.nature)
    log(user.heldItem)
    log(user.ability)
    log(user.gender)
    log(user.form)
    log(user.displayName)
    log(user.status)
    log(user.asleepRounds)
    log(user.level)
    log(user.hp)
    log(user.attack)
    log(user.defense)
    log(user.specialAttack)
    log(user.specialDefense)
    log(user.speed)
    log(user.currentHP)
    showBattleText("Hello, I'm a "..user.displayName..".")
    return 0
    -- write("I'm the Wise Rock.")
    -- showText("It's time to battle.")
    -- log("Lorem ipsum dolor sit amet")
end
