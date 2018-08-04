#include "overworld/render-map.hpp"

#include "components/Map.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"

constexpr int TILE_SIZE = 32;

void renderMap(
    sf::RenderWindow& window,
    engine::entitysystem::ComponentManager& manager,
    engine::resourcesystem::ResourceStorage& storage
) {
    using engine::entitysystem::Entity;

    manager.forEachEntity<Map>(
        [&](
            Entity entity,
            Map& map
        ) {
            for (size_t i = 0; i < map.tiles.size(); ++i) {
                float x = (i % map.widthInTiles) * TILE_SIZE;
                float y = (i / map.widthInTiles) * TILE_SIZE;
                Tile& tile = map.tiles.at(i);
                tile.sprite.setPosition({x, y});
                window.draw(tile.sprite);
            }
        }
    );
}
