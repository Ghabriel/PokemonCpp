#include "overworld/render-map.hpp"

#include "components/Map.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"
#include "Settings.hpp"

void renderMapLayer(
    MapLayer layer,
    sf::RenderWindow& window,
    engine::entitysystem::ComponentManager& manager,
    engine::resourcesystem::ResourceStorage& storage
) {
    using engine::entitysystem::Entity;
    static const auto tileSize = storage.get<Settings>("settings").getTileSize();
    auto layerValue = static_cast<size_t>(layer);

    manager.forEachEntity<Map>(
        [&](
            Entity entity,
            Map& map
        ) {
            for (size_t i = 0; i < map.tiles.size(); ++i) {
                float x = (i % map.widthInTiles) * tileSize;
                float y = (i / map.widthInTiles) * tileSize;
                Tile& tile = map.tiles.at(i);

                if (tile.sprites.size() > layerValue) {
                    sf::Sprite& sprite = tile.sprites.at(layerValue);
                    sprite.setPosition({x, y});
                    window.draw(sprite);
                }
            }
        }
    );
}
