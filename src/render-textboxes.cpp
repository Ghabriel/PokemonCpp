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
    constexpr int textBoxMargin = 5;
    constexpr int textMargin = 5;
    Camera& camera = storage.get<Camera>("camera");
    const int textBoxWidth = camera.width - 2 * textBoxMargin;
    const int textBoxHeight = camera.height / 5;
    const int textBoxX = textBoxMargin;
    const int textBoxY = camera.height - textBoxHeight - textBoxMargin;
    const int textX = textBoxX + textMargin;
    const int textY = textBoxY + textMargin;

    manager.forEachEntity<TextBox>(
        [&](
            Entity entity,
            TextBox& textBox
        ) {

            sf::RectangleShape rect(sf::Vector2f(textBoxWidth, textBoxHeight));
            rect.setPosition(textBoxX, textBoxY);
            rect.setFillColor(sf::Color::White);
            window.draw(rect);

            sf::Text text(textBox.content, storage.get<sf::Font>("font-arial"));
            text.setPosition(textX, textY);
            text.setFillColor(sf::Color::Black);
            text.setCharacterSize(30);
            window.draw(text);
        }
    );
}
