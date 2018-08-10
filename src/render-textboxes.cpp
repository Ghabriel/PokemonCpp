#include "render-textboxes.hpp"

#include <cmath>
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
    constexpr int textBoxMinMargin = 5;
    constexpr int textMargin = 16;
    Camera& camera = storage.get<Camera>("camera");
    const int textBoxMargin = (camera.width - 16 * std::floor((camera.width - 2 * textBoxMinMargin) / 16)) / 2;
    const int textBoxWidth = 16 * std::floor((camera.width - 2 * textBoxMinMargin) / 16);
    const int textBoxHeight = camera.height / 5 - ((camera.height / 5) % 16);
    const int textBoxX = textBoxMargin;
    const int textBoxY = camera.height - textBoxHeight - textBoxMargin;
    const int textX = textBoxX + textMargin;
    const int textY = textBoxY + textMargin;

    manager.forEachEntity<TextBox>(
        [&](
            Entity entity,
            TextBox& textBox
        ) {
            sf::Texture& textBoxSkin = storage.get<sf::Texture>("text-skin-1");
            sf::Sprite sprite00(textBoxSkin);
            sprite00.setTextureRect(sf::IntRect{0, 0, 16, 16});
            sf::Sprite sprite10(textBoxSkin);
            sprite10.setTextureRect(sf::IntRect{16, 0, 16, 16});
            sf::Sprite sprite20(textBoxSkin);
            sprite20.setTextureRect(sf::IntRect{32, 0, 16, 16});
            sf::Sprite sprite01(textBoxSkin);
            sprite01.setTextureRect(sf::IntRect{0, 16, 16, 16});
            sf::Sprite sprite21(textBoxSkin);
            sprite21.setTextureRect(sf::IntRect{32, 16, 16, 16});
            sf::Sprite sprite02(textBoxSkin);
            sprite02.setTextureRect(sf::IntRect{0, 32, 16, 16});
            sf::Sprite sprite12(textBoxSkin);
            sprite12.setTextureRect(sf::IntRect{16, 32, 16, 16});
            sf::Sprite sprite22(textBoxSkin);
            sprite22.setTextureRect(sf::IntRect{32, 32, 16, 16});

            sf::RectangleShape rect(sf::Vector2f(textBoxWidth - 10, textBoxHeight - 10));
            rect.setPosition(textBoxX + 5, textBoxY + 5);
            rect.setFillColor(sf::Color::White);
            window.draw(rect);

            sprite00.setPosition(textBoxX, textBoxY);
            window.draw(sprite00);
            sprite20.setPosition(textBoxX + textBoxWidth - 16, textBoxY);
            window.draw(sprite20);
            sprite02.setPosition(textBoxX, textBoxY + textBoxHeight - 16);
            window.draw(sprite02);
            sprite22.setPosition(textBoxX + textBoxWidth - 16, textBoxY + textBoxHeight - 16);
            window.draw(sprite22);

            int horizontalCentralTiles = (textBoxWidth - 32) / 16;
            for (int i = 1; i <= horizontalCentralTiles; ++i) {
                sprite10.setPosition(textBoxX + i * 16, textBoxY);
                window.draw(sprite10);
                sprite12.setPosition(textBoxX + i * 16, textBoxY + textBoxHeight - 16);
                window.draw(sprite12);
            }

            int verticalCentralTiles = (textBoxHeight - 32) / 16;
            for (int i = 1; i <= verticalCentralTiles; ++i) {
                sprite01.setPosition(textBoxX, textBoxY + i * 16);
                window.draw(sprite01);
                sprite21.setPosition(textBoxX + textBoxWidth - 16, textBoxY + i * 16);
                window.draw(sprite21);
            }

            sf::Text text(textBox.content, storage.get<sf::Font>("font-arial"));
            text.setPosition(textX, textY);
            text.setFillColor(sf::Color::Black);
            text.setCharacterSize(30);
            window.draw(text);
        }
    );
}
