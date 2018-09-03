#include "test-utils.hpp"
#include "engine/testing/include.hpp"

using test::before;
using test::describe;
using test::it;

void testBattle() {
    loadTestSuite();

    describe("Tackle", [&] {
        it("does damage", [&] {
            auto rattata = wrap(generatePokemon("Rattata", 3).build());
            auto pidgey = wrap(generatePokemon("Pidgey", 3).build());

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
