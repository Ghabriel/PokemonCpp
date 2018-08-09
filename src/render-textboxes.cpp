#include "render-textboxes.hpp"

#include "components/Camera.hpp"
#include "components/TextBox.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"

void renderTextBoxes(
    sf::RenderWindow& window,
    engine::entitysystem::ComponentManager& manager,
    engine::resourcesystem::ResourceStorage& storage
) {
    using engine::entitysystem::Entity;

    manager.forEachEntity<TextBox>(
        [&](
            Entity entity,
            TextBox& textBox
        ) {
            sf::Text text(textBox.content, storage.get<sf::Font>("font-arial"));
            text.setCharacterSize(30);
            window.draw(text);
        }
    );
}
