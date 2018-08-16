#include "render-textboxes.hpp"

#include "components/Battle.hpp"
#include "components/BattleActionSelection.hpp"
#include "components/Camera.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/resource-system/include.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::resourcesystem::ResourceStorage;

void renderAllyPokemon(
    sf::RenderWindow& window,
    Camera& camera,
    ResourceStorage& storage,
    Pokemon& pokemon
) {
    sf::Sprite sprite(storage.get<sf::Texture>("pokemon-back-" + pokemon.species));
    float scaledHeight = camera.height / 5;
    sprite.scale(scaledHeight / 64, scaledHeight / 64);
    sprite.setPosition(camera.width / 10, 3 * camera.height / 5);
    window.draw(sprite);
}

void renderFoePokemon(
    sf::RenderWindow& window,
    Camera& camera,
    ResourceStorage& storage,
    Pokemon& pokemon
) {
    sf::Sprite sprite(storage.get<sf::Texture>("pokemon-front-" + pokemon.species));
    float scaledHeight = camera.height / 5;
    sprite.scale(scaledHeight / 64, scaledHeight / 64);
    sprite.setPosition(7 * camera.width / 10, camera.height / 10);
    window.draw(sprite);
}

void renderInfoCard(
    sf::RenderWindow& window,
    ResourceStorage& storage,
    Pokemon& pokemon,
    float baseX,
    float baseY,
    bool isAlly
) {
    std::string content = std::to_string(pokemon.currentHP) + "/" + std::to_string(pokemon.stats[0]);
    sf::Text text(content, storage.get<sf::Font>("font-arial"));
    text.setPosition(baseX, baseY);
    window.draw(text);
}

void renderBattle(
    sf::RenderWindow& window,
    engine::entitysystem::ComponentManager& manager,
    ResourceStorage& storage
) {
    using engine::entitysystem::Entity;

    manager.forEachEntity<Battle>(
        [&](
            Entity entity,
            Battle& battle
        ) {
            Camera& camera = storage.get<Camera>("camera");
            renderAllyPokemon(window, camera, storage, battle.playerPokemon);
            renderInfoCard(
                window,
                storage,
                battle.playerPokemon,
                7 * camera.width / 10,
                3 * camera.height / 5,
                true
            );
            renderFoePokemon(window, camera, storage, battle.opponentPokemon);
            renderInfoCard(
                window,
                storage,
                battle.opponentPokemon,
                camera.width / 10,
                camera.height / 7,
                false
            );
        }
    );

    // manager.forEachEntity<BattleActionSelection>(
    //     [&](
    //         Entity entity,
    //         BattleActionSelection& selection
    //     ) {
    //         // TODO
    //     }
    // );
}
