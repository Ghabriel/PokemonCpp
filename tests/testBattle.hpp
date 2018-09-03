#include "battle/BattleController.hpp"
#include "battle/data/Pokemon.hpp"
#include "battle/helpers/generate-pokemon.hpp"
#include "components/battle/Battle.hpp"
#include "CoreStructures.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/input-system/include.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/state-system/include.hpp"
#include "init/load-input-tracker.hpp"
#include "init/load-resources.hpp"
#include "ResourceFiles.hpp"
#include "Settings.hpp"

#include "engine/testing/include.hpp"

using test::before;
using test::describe;
using test::it;

engine::entitysystem::ComponentManager componentManager;
engine::inputsystem::InputTracker inputTracker(loadInputTracker(ResourceFiles::CONTROLS));
engine::inputsystem::InputDispatcher inputDispatcher(inputTracker);
engine::resourcesystem::ResourceStorage resourceStorage;
engine::statesystem::StateMachine stateMachine;
double timeBetweenTicks = 999999;

using engine::entitysystem::Entity;

CoreStructures createGameData() {
    return {
        &componentManager,
        &inputDispatcher,
        &resourceStorage,
        &stateMachine,
        &timeBetweenTicks
    };
}

struct TestData {
    Entity battleEntity;
    BattleController battleController;
};

Pokemon generatePokemon(const std::string& species, int level) {
    return generatePokemon(resourceStorage, species, level);
}

TestData prepareTestData(CoreStructures& gameData) {
    TestData result;
    result.battleEntity = componentManager.createEntity();
    result.battleController = BattleController(result.battleEntity, gameData);
    return result;
}

Entity wrap(const Pokemon& pokemon) {
    auto entity = componentManager.createEntity();
    componentManager.addComponent(entity, pokemon);
    return entity;
}

void setPokemon(TestData& testData, Entity player, Entity opponent) {
    componentManager.addComponent(
        testData.battleEntity,
        Battle{player, opponent}
    );
}

TestData prepareBattle(CoreStructures& gameData, Entity player, Entity opponent) {
    TestData testData = prepareTestData(gameData);
    setPokemon(testData, player, opponent);
    testData.battleController.startBattle();
    return testData;
}

Entity getPlayerPokemon(const TestData& testData) {
    return componentManager.getData<Battle>(testData.battleEntity).playerPokemon;
}

Entity getOpponentPokemon(const TestData& testData) {
    return componentManager.getData<Battle>(testData.battleEntity).opponentPokemon;
}

int getMoveIndex(Entity pokemon, const std::string& moveId) {
    auto moves = componentManager.getData<std::vector<Move*>>(pokemon);

    if (moveId == "Struggle") {
        return -2;
    }

    for (size_t i = 0; i < moves.size(); ++i) {
        if (moves[i]->id == moveId) {
            return i;
        }
    }

    return -1;
}

UsedMove wrapMove(Entity user, Entity target, int moveIndex) {
    Move* move = (moveIndex == -2)
        ? &resourceStorage.get<Move>("move-Struggle")
        : componentManager.getData<std::vector<Move*>>(user)[moveIndex];

    return {
        user,
        target,
        move,
        moveIndex
    };
}

UsedMove wrapMove(Entity user, Entity target, const std::string& moveId) {
    return wrapMove(user, target, getMoveIndex(user, moveId));
}

void processAllEvents(TestData& testData) {
    while (testData.battleController.hasPendingEvents()) {
        testData.battleController.tick();
    }
}

void processTurn(TestData& testData, const std::deque<UsedMove>& usedMoves) {
    testData.battleController.processTurn(usedMoves);
    processAllEvents(testData);
}

Pokemon& pokemon(Entity entity) {
    return componentManager.getData<Pokemon>(entity);
}

int currentHP(Entity entity) {
    return pokemon(entity).currentHP;
}

void testBattle() {
    CoreStructures gameData = createGameData();
    resourceStorage.store("settings", Settings{});
    loadResources(resourceStorage);

    describe("Tackle", [&] {
        it("does damage", [&] {
            Entity rattata = wrap(generatePokemon("Rattata", 3));
            Entity pidgey = wrap(generatePokemon("Pidgey", 3));

            TestData testData = prepareBattle(gameData, rattata, pidgey);
            processTurn(testData, {
                wrapMove(rattata, pidgey, "Tackle"),
                wrapMove(pidgey, rattata, "Tackle")
            });

            expect(currentHP(rattata)).toBe(10);
            expect(currentHP(pidgey)).toBe(10);
        });
    });
}
