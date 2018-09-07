#include "render-textboxes.hpp"

#include "battle/data/Pokemon.hpp"
#include "components/battle/Battle.hpp"
#include "components/battle/BattleActionSelection.hpp"
#include "components/battle/Fainted.hpp"
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

sf::Color getHealthBarColor(float percentage) {
    if (percentage > 0.5) {
        return sf::Color::Green;
    }

    if (percentage > 0.25) {
        return sf::Color::Yellow;
    }

    return sf::Color::Red;
}

void renderInfoCard(
    sf::RenderWindow& window,
    ResourceStorage& storage,
    Pokemon& pokemon,
    float baseX,
    float baseY,
    bool isAlly
) {
    constexpr size_t hpContainerWidth = 200;
    constexpr size_t hpContainerHeight = 18;
    constexpr size_t hpContainerYOffset = 25;
    constexpr size_t hpMargin = 2;

    sf::Font& font = storage.get<sf::Font>("font-arial");
    sf::Text name(pokemon.displayName, font);
    name.setCharacterSize(20);
    name.setFillColor(sf::Color::White);
    name.setOutlineColor(sf::Color::Black);
    name.setOutlineThickness(1);
    name.setPosition(baseX, baseY);
    window.draw(name);

    sf::Text level("Lv." + std::to_string(pokemon.level), font);
    level.setCharacterSize(20);
    level.setFillColor(sf::Color::White);
    level.setOutlineColor(sf::Color::Black);
    level.setOutlineThickness(1);
    level.setPosition(baseX + hpContainerWidth, baseY);
    window.draw(level);

    sf::RectangleShape hpContainer({hpContainerWidth, hpContainerHeight});
    hpContainer.setPosition(baseX, baseY + hpContainerYOffset);
    hpContainer.setFillColor(sf::Color::Black);
    window.draw(hpContainer);

    float hpPercentage = pokemon.currentHP / pokemon.stats[0];
    sf::RectangleShape hpBar({
        (hpContainerWidth - 2 * hpMargin) * hpPercentage,
        hpContainerHeight - 2 * hpMargin
    });
    hpBar.setPosition(baseX + hpMargin, baseY + hpContainerYOffset + hpMargin);
    hpBar.setFillColor(getHealthBarColor(hpPercentage));
    window.draw(hpBar);

    if (isAlly) {
        size_t hpTextY = baseY + hpContainerYOffset + hpContainerHeight;
        constexpr size_t expContainerWidth = 200;
        constexpr size_t expContainerHeight = 5;
        constexpr size_t expContainerYOffset = 30;
        constexpr size_t expMargin = 1;

        sf::Text hpText(
            std::to_string(static_cast<int>(pokemon.currentHP)) +
                "/" + std::to_string(pokemon.stats[0]),
            font
        );
        hpText.setCharacterSize(25);
        hpText.setFillColor(sf::Color::White);
        hpText.setOutlineColor(sf::Color::Black);
        hpText.setOutlineThickness(1);
        hpText.setPosition(baseX, hpTextY);
        window.draw(hpText);

        sf::RectangleShape expContainer({expContainerWidth, expContainerHeight});
        expContainer.setPosition(baseX, hpTextY + expContainerYOffset);
        expContainer.setFillColor(sf::Color::Black);
        window.draw(expContainer);

        float expPercentage = 0.25; // TODO
        sf::RectangleShape expBar({
            (expContainerWidth - 2 * expMargin) * expPercentage,
            expContainerHeight - 2 * expMargin
        });
        expBar.setPosition(baseX + expMargin, hpTextY + expContainerYOffset + expMargin);
        expBar.setFillColor(sf::Color::Cyan);
        window.draw(expBar);
    }
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
            sf::Sprite background(storage.get<sf::Texture>("battle-bg-1"));
            background.setScale(camera.width / 512.0, camera.height / 288.0);
            background.setPosition(0, 0);
            window.draw(background);

            // TODO: handle Double Battles
            if (!manager.hasComponent<Fainted>(battle.playerTeam[0])) {
                Pokemon& playerPokemon = manager.getData<Pokemon>(battle.playerTeam[0]);
                renderAllyPokemon(window, camera, storage, playerPokemon);
                renderInfoCard(
                    window,
                    storage,
                    playerPokemon,
                    6 * camera.width / 10,
                    3 * camera.height / 5,
                    true
                );
            }

            if (!manager.hasComponent<Fainted>(battle.opponentTeam[0])) {
                Pokemon& opponentPokemon = manager.getData<Pokemon>(battle.opponentTeam[0]);
                renderFoePokemon(window, camera, storage, opponentPokemon);
                renderInfoCard(
                    window,
                    storage,
                    opponentPokemon,
                    camera.width / 10,
                    camera.height / 7,
                    false
                );
            }
        }
    );
}
