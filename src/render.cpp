#include "render.hpp"

#include <SFML/Graphics.hpp>
#include "battle/render-battle.hpp"
#include "components/DrawableVector.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"
#include "engine/utils/misc/Menu.hpp"
#include "overworld/render-map.hpp"
#include "MapLayer.hpp"
#include "render-textboxes.hpp"

using engine::entitysystem::Entity;
using engine::utils::Menu;

void renderMenus(
    sf::RenderWindow& window,
    engine::entitysystem::ComponentManager& manager,
    engine::resourcesystem::ResourceStorage& storage
) {
    static sf::Font& arial = storage.get<sf::Font>("font-arial");
    constexpr size_t fontSize = 30;
    sf::Vector2u windowSize = window.getSize();
    size_t windowWidth = windowSize.x;
    size_t windowHeight = windowSize.y;

    manager.forEachEntity<Menu>(
        [&](Entity entity, Menu& menu) {
            size_t focusedIndex = menu.getFocusedOption();
            size_t numOptions = menu.size();
            size_t optionSpacing = windowHeight / numOptions;
            size_t padding = optionSpacing / 2 - fontSize;
            size_t i = 0;

            menu.forEachLabel([&](const std::string& label) {
                sf::Text text(label, arial, fontSize);
                size_t x = (windowWidth - text.getLocalBounds().width) / 2;
                size_t y = i * optionSpacing + padding;
                text.setPosition(x, y);
                text.setFillColor(i == focusedIndex ? sf::Color::Green : sf::Color::White);
                window.draw(text);
                ++i;
            });
        }
    );
}

void renderLoopingAnimations(
    sf::RenderWindow& window,
    engine::entitysystem::ComponentManager& manager
) {
    using namespace engine::spritesystem;

    manager.forEachEntity<LoopingAnimationData, AnimationPlaybackData>(
        [&](
            Entity entity,
            LoopingAnimationData& animationData,
            AnimationPlaybackData&
        ) {
            window.draw(animationData.sprite);
        }
    );
}

void renderDrawableVectors(
    sf::RenderWindow& window,
    engine::entitysystem::ComponentManager& manager,
    engine::resourcesystem::ResourceStorage& storage
) {
    manager.forEachEntity<DrawableVector>(
        [&](
            Entity entity,
            DrawableVector& vector
        ) {
            for (const auto& shape : vector.shapes) {
                window.draw(*shape);
            }
        }
    );
}

void render(
    sf::RenderWindow& window,
    engine::entitysystem::ComponentManager& manager,
    engine::resourcesystem::ResourceStorage& storage
) {
    renderMapLayer(MapLayer::Terrain, window, manager, storage);
    renderMapLayer(MapLayer::Objects, window, manager, storage);
    renderMenus(window, manager, storage);
    renderBattle(window, manager, storage);
    renderLoopingAnimations(window, manager);
    renderMapLayer(MapLayer::Foreground, window, manager, storage);
    renderTextBoxes(window, manager, storage);
    renderDrawableVectors(window, manager, storage);
}
