#include "render-textboxes.hpp"

#include <array>
#include <cmath>
#include <sstream>
#include "components/battle/BattleActionSelection.hpp"
#include "components/battle/BattleMoveSelection.hpp"
#include "components/Camera.hpp"
#include "components/TextBox.hpp"
#include "engine/entity-system/include.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::resourcesystem::ResourceStorage;

using TextBoxSkinGrid = std::array<std::array<sf::Sprite, 3>, 3>;

constexpr int textBoxMinMargin = 5;
constexpr int textMargin = 16;
int textBoxMargin;
int textBoxWidth;
int textBoxHeight;
int textBoxX;
int textBoxY;

void updateTextBoxVariables(Camera& camera, int boxWidth) {
    textBoxMargin = (boxWidth - 16 * std::floor((boxWidth - 2 * textBoxMinMargin) / 16)) / 2;
    textBoxWidth = 16 * std::floor((boxWidth - 2 * textBoxMinMargin) / 16);
    textBoxHeight = camera.height / 5 - ((camera.height / 5) % 16);
    textBoxX = camera.x + textBoxMargin;
    textBoxY = camera.y + camera.height - textBoxHeight - textBoxMargin;
}

void updateTextBoxVariables(Camera& camera) {
    updateTextBoxVariables(camera, camera.width);
}

TextBoxSkinGrid getTextBoxSkinSprites(
    engine::resourcesystem::ResourceStorage& storage
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

    return {{
        {sprite00, sprite01, sprite02},
        {sprite10, {}, sprite12},
        {sprite20, sprite21, sprite22}
    }};
}

void renderBox(
    sf::RenderWindow& window,
    ResourceStorage& storage
) {
    static TextBoxSkinGrid sprites = getTextBoxSkinSprites(storage);

    sf::RectangleShape rect(sf::Vector2f(textBoxWidth - 10, textBoxHeight - 10));
    rect.setPosition(textBoxX + 5, textBoxY + 5);
    rect.setFillColor(sf::Color::White);
    window.draw(rect);

    sprites[0][0].setPosition(textBoxX, textBoxY);
    window.draw(sprites[0][0]);
    sprites[2][0].setPosition(textBoxX + textBoxWidth - 16, textBoxY);
    window.draw(sprites[2][0]);
    sprites[0][2].setPosition(textBoxX, textBoxY + textBoxHeight - 16);
    window.draw(sprites[0][2]);
    sprites[2][2].setPosition(textBoxX + textBoxWidth - 16, textBoxY + textBoxHeight - 16);
    window.draw(sprites[2][2]);

    int horizontalCentralTiles = (textBoxWidth - 32) / 16;
    for (int i = 1; i <= horizontalCentralTiles; ++i) {
        sprites[1][0].setPosition(textBoxX + i * 16, textBoxY);
        window.draw(sprites[1][0]);
        sprites[1][2].setPosition(textBoxX + i * 16, textBoxY + textBoxHeight - 16);
        window.draw(sprites[1][2]);
    }

    int verticalCentralTiles = (textBoxHeight - 32) / 16;
    for (int i = 1; i <= verticalCentralTiles; ++i) {
        sprites[0][1].setPosition(textBoxX, textBoxY + i * 16);
        window.draw(sprites[0][1]);
        sprites[2][1].setPosition(textBoxX + textBoxWidth - 16, textBoxY + i * 16);
        window.draw(sprites[2][1]);
    }
}

sf::Text buildTextInstance(ResourceStorage& storage) {
    sf::Text text("", storage.get<sf::Font>("font-arial"));
    text.setFillColor(sf::Color::Black);
    text.setCharacterSize(30);

    return text;
}

void adjustTextPosition(sf::Text& text) {
    const int textX = textBoxX + textMargin;
    const int textY = textBoxY + textMargin;
    text.setPosition(textX, textY);
}

float getSimulatedTextWidth(sf::Text& instance, const std::string& content) {
    instance.setString(content);
    return instance.getGlobalBounds().width;
}

float getSimulatedTextHeight(sf::Text& instance, const std::string& content) {
    instance.setString(content);
    return instance.getGlobalBounds().height;
}

bool invalidateTextCacheOnResize(
    Camera& camera,
    TextBox& textBox
) {
    static int cachedCameraWidth = 0;
    static int cachedCameraHeight = 0;

    if (camera.width != cachedCameraWidth || camera.height != cachedCameraHeight) {
        textBox.cachedParsedText = "";
        textBox.cachedContentSize = 0;
        cachedCameraWidth = camera.width;
        cachedCameraHeight = camera.height;
        return true;
    }

    return false;
}

void updateTextCache(sf::Text& text, TextBox& textBox) {
    const int textMaxWidth = textBoxWidth - 2 * textMargin;
    auto overflowsWithContent = [&](const std::string& content) {
        return getSimulatedTextWidth(text, content) > textMaxWidth;
    };

    const int textMaxHeight = textBoxHeight - 2 * textMargin;
    auto overflowsHeightWithContent = [&](const std::string& content) {
        return getSimulatedTextHeight(text, content) > textMaxHeight;
    };

    std::stringstream ss;
    ss << textBox.cachedParsedText;

    const size_t contentSize = textBox.content.size();
    const size_t fullContentSize = textBox.fullContent.size();

    for (size_t i = textBox.cachedContentSize; i < contentSize; ++i) {
        char ch = textBox.content.at(i);

        if (ch == ' ') {
            if (!overflowsWithContent(ss.str() + std::to_string(ch))) {
                ss << ch;
            }

            continue;
        }

        std::stringstream wordEnd;
        size_t j = i;
        while (j < fullContentSize && textBox.fullContent.at(j) != ' ') {
            wordEnd << textBox.fullContent.at(j);
            ++j;
        }

        std::string wordEndStr = wordEnd.str();

        if (overflowsWithContent(ss.str() + wordEndStr)) {
            ss << '\n';

            if (overflowsHeightWithContent(ss.str() + wordEndStr)) {
                textBox.overflow = true;
                break;
            }
        }

        ss << ch;
    }

    textBox.cachedParsedText = ss.str();
    textBox.cachedContentSize = contentSize;
}

void renderText(
    sf::RenderWindow& window,
    Camera& camera,
    ResourceStorage& storage,
    TextBox& textBox
) {
    static sf::Text text = buildTextInstance(storage);
    adjustTextPosition(text);

    if (
        !textBox.overflow && (
            invalidateTextCacheOnResize(camera, textBox) ||
            textBox.content.size() != textBox.cachedContentSize
        )
    ) {
        updateTextCache(text, textBox);
    }

    text.setString(textBox.cachedParsedText);
    window.draw(text);
}

void renderText(
    sf::RenderWindow& window,
    ResourceStorage& storage,
    const std::string& content
) {
    static sf::Text text = buildTextInstance(storage);
    adjustTextPosition(text);
    text.setString(content);
    window.draw(text);
}

void renderActionFocusBox(
    sf::RenderWindow& window,
    const BattleActionSelection& selection,
    int firstRowY
) {
    float focusBoxWidth = (selection.focusedOption % 2 == 0)
        ? textBoxWidth / 2 - 10
        : textBoxWidth / 2 - 30;
    float focusBoxX = (selection.focusedOption % 2 == 0)
        ? textBoxX
        : textBoxX + textBoxWidth / 2;
    float focusBoxY = (selection.focusedOption <= 1)
        ? firstRowY
        : textBoxY;

    sf::RectangleShape focusBox({focusBoxWidth, 35});
    focusBox.setOutlineColor(sf::Color::Red);
    focusBox.setOutlineThickness(1);
    focusBox.setFillColor(sf::Color::Transparent);
    focusBox.setPosition({focusBoxX + textMargin - 1, focusBoxY + textMargin + 5});
    window.draw(focusBox);
}

void renderMoveFocusBox(
    sf::RenderWindow& window,
    const BattleMoveSelection& selection,
    int firstRowY
) {
    float focusBoxWidth = (selection.focusedOption % 2 == 0)
        ? textBoxWidth / 2 - 13
        : textBoxWidth / 2 - 40;
    float focusBoxX = (selection.focusedOption % 2 == 0)
        ? textBoxX
        : textBoxX + textBoxWidth / 2 - 10;
    float focusBoxY = (selection.focusedOption <= 1)
        ? firstRowY
        : textBoxY;

    sf::RectangleShape focusBox({focusBoxWidth, 35});
    focusBox.setOutlineColor(sf::Color::Red);
    focusBox.setOutlineThickness(1);
    focusBox.setFillColor(sf::Color::Transparent);
    focusBox.setPosition({focusBoxX + textMargin, focusBoxY + textMargin + 3});
    window.draw(focusBox);
}

std::string displayMove(const std::optional<MoveDisplayInfo>& move) {
    return move ? move->displayName : "-";
}

void renderTextBoxes(
    sf::RenderWindow& window,
    engine::entitysystem::ComponentManager& manager,
    ResourceStorage& storage
) {
    using engine::entitysystem::Entity;
    Camera& camera = storage.get<Camera>("camera");
    updateTextBoxVariables(camera);

    manager.forEachEntity<TextBox>(
        [&](
            Entity entity,
            TextBox& textBox
        ) {
            renderBox(window, storage);
            renderText(window, camera, storage, textBox);
        }
    );

    manager.forEachEntity<BattleActionSelection>(
        [&](
            Entity entity,
            BattleActionSelection& selection
        ) {
            // Text box
            int firstBoxWidth = camera.width - selection.optionBoxWidth;
            updateTextBoxVariables(camera, firstBoxWidth);
            renderBox(window, storage);
            renderText(window, storage, selection.text);

            // Options box
            textBoxWidth = 16 * std::floor((selection.optionBoxWidth - 2 * textBoxMinMargin) / 16);
            textBoxX += firstBoxWidth - textBoxMargin;
            renderBox(window, storage);

            // Options
            renderText(window, storage, selection.options[0]);

            textBoxX += textBoxWidth / 2;
            renderText(window, storage, selection.options[1]);

            int firstRowY = textBoxY;
            textBoxY = camera.y + camera.height - 25 - textBoxMargin - 3 * textMargin;
            renderText(window, storage, selection.options[3]);

            textBoxX -= textBoxWidth / 2;
            renderText(window, storage, selection.options[2]);

            renderActionFocusBox(window, selection, firstRowY);
        }
    );

    manager.forEachEntity<BattleMoveSelection>(
        [&](
            Entity entity,
            BattleMoveSelection& selection
        ) {
            // Move box
            int firstBoxWidth = camera.width - selection.optionBoxWidth;
            int firstBoxX = textBoxX;
            updateTextBoxVariables(camera, firstBoxWidth);
            renderBox(window, storage);

            // Moves
            renderText(window, storage, displayMove(selection.moves[0]));
            textBoxX += textBoxWidth / 2;
            renderText(window, storage, displayMove(selection.moves[1]));
            int firstRowY = textBoxY;
            int secondRowY = camera.y + camera.height - 25 - textBoxMargin - 3 * textMargin;
            textBoxY = secondRowY;
            renderText(window, storage, displayMove(selection.moves[3]));
            textBoxX -= textBoxWidth / 2;
            renderText(window, storage, displayMove(selection.moves[2]));

            // Move info box
            textBoxY = firstRowY;
            textBoxWidth = 16 * std::floor((selection.optionBoxWidth - 2 * textBoxMinMargin) / 16);
            textBoxX = firstBoxX + firstBoxWidth - textBoxMargin;
            renderBox(window, storage);

            if (selection.moves[selection.focusedOption]) {
                // Move info
                MoveDisplayInfo& focusedMove = *selection.moves[selection.focusedOption];
                renderText(window, storage, "PP " + std::to_string(focusedMove.pp) + "/" + std::to_string(focusedMove.maxPP));
                textBoxY = secondRowY;
                renderText(window, storage, focusedMove.type);
            }

            textBoxX = firstBoxX;
            textBoxY = secondRowY;
            textBoxWidth = firstBoxWidth;
            renderMoveFocusBox(window, selection, firstRowY);
        }
    );
}
