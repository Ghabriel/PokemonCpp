#include "battle/BattleController.hpp"
#include "battle/data/Pokemon.hpp"
#include "battle/helpers/generate-pokemon.hpp"
#include "battle/text-providers/NullTextProvider.hpp"
#include "components/battle/Battle.hpp"
#include "CoreStructures.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/input-system/include.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/state-system/include.hpp"
#include "init/load-input-tracker.hpp"
#include "init/load-resources.hpp"
#include "PokemonBuilder.hpp"
#include "printer.hpp"
#include "ResourceFiles.hpp"
#include "Settings.hpp"

using engine::entitysystem::Entity;

inline engine::entitysystem::ComponentManager componentManager;
inline engine::inputsystem::InputTracker inputTracker(loadInputTracker(ResourceFiles::CONTROLS));
inline engine::inputsystem::InputDispatcher inputDispatcher(inputTracker);
inline engine::resourcesystem::ResourceStorage resourceStorage;
inline engine::statesystem::StateMachine stateMachine;
inline double timeBetweenTicks = 999999;

inline CoreStructures gameData = {
    &componentManager,
    &inputDispatcher,
    &resourceStorage,
    &stateMachine,
    &timeBetweenTicks
};

struct TestData {
    Entity battleEntity;
    BattleController battleController;
};

namespace {
    TestData prepareTestData(CoreStructures& gameData);
    void setPokemon(TestData&, Entity player, Entity opponent);
}

// Should be called exactly once, before all tests
void loadTestSuite() {
    resourceStorage.store("settings", Settings{});
    loadResources(resourceStorage);
}

// Environment configuration
TestData prepareBattle(CoreStructures& gameData, Entity player, Entity opponent);

// Battle processing
void processTurn(TestData&, const std::deque<UsedMove>& usedMoves);
void processAllEvents(TestData&);

// Getters for assertions
Pokemon& pokemon(Entity);
int currentHP(Entity);
int getMoveIndex(Entity pokemon, const std::string& moveId);

// Pokémon builder
PokemonBuilder generatePokemon(const std::string& species, int level);

Entity wrap(const Pokemon& pokemon);
UsedMove wrapMove(Entity user, Entity target, int moveIndex);
UsedMove wrapMove(Entity user, Entity target, const std::string& moveId);


namespace {
    inline std::unique_ptr<TextProvider> textProvider(new NullTextProvider());

    TestData prepareTestData(CoreStructures& gameData) {
        TestData result;
        result.battleEntity = componentManager.createEntity();
        result.battleController = BattleController(
            result.battleEntity,
            *textProvider,
            gameData
        );
        return result;
    }

    void setPokemon(TestData& testData, Entity player, Entity opponent) {
        componentManager.addComponent(
            testData.battleEntity,
            Battle{player, opponent}
        );
    }
}

TestData prepareBattle(CoreStructures& gameData, Entity player, Entity opponent) {
    TestData testData = prepareTestData(gameData);
    setPokemon(testData, player, opponent);
    testData.battleController.startBattle();
    return testData;
}

void processTurn(TestData& testData, const std::deque<UsedMove>& usedMoves) {
    testData.battleController.processTurn(usedMoves);
    processAllEvents(testData);
}

void processAllEvents(TestData& testData) {
    while (testData.battleController.hasPendingEvents()) {
        testData.battleController.tick();
    }
}

Pokemon& pokemon(Entity entity) {
    return componentManager.getData<Pokemon>(entity);
}

int currentHP(Entity entity) {
    return pokemon(entity).currentHP;
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

PokemonBuilder generatePokemon(const std::string& species, int level) {
    return PokemonBuilder(resourceStorage, species, level);
}

Entity wrap(const Pokemon& pokemon) {
    auto entity = componentManager.createEntity();
    componentManager.addComponent(entity, pokemon);
    return entity;
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
