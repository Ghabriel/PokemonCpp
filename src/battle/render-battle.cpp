#include "render-textboxes.hpp"

#include "components/Battle.hpp"
#include "components/Camera.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/resource-system/include.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::resourcesystem::ResourceStorage;

void renderBattle(
    sf::RenderWindow& window,
    engine::entitysystem::ComponentManager& manager,
    ResourceStorage& storage
) {
    using engine::entitysystem::Entity;
    // Camera& camera = storage.get<Camera>("camera");

    manager.forEachEntity<Battle>(
        [&](
            Entity entity,
            Battle& battle
        ) {
            // Pokemon& player = battle.playerPokemon;
            Pokemon& opponent = battle.opponentPokemon;
            sf::Sprite opponentSprite(storage.get<sf::Texture>("pokemon-front-" + opponent.species));
            window.draw(opponentSprite);
        }
    );
}
