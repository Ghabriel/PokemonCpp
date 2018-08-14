#include "render-textboxes.hpp"

#include "components/Battle.hpp"
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
    sprite.setPosition(camera.width / 10, 4 * camera.height / 5);
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
    sprite.setPosition(4 * camera.width / 5, 0);
    window.draw(sprite);
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
            renderFoePokemon(window, camera, storage, battle.opponentPokemon);
        }
    );
}
