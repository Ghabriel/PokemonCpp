#include "init/load-resources.hpp"

#include <cassert>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "engine/resource-system/include.hpp"
#include "engine/sfml/sound-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"

using engine::resourcesystem::ResourceStorage;

void loadFonts(ResourceStorage& storage) {
    sf::Font arial;
    assert(arial.loadFromFile("resources/fonts/arial.ttf"));
    storage.store("font-arial", arial);
}

void loadTextures(ResourceStorage& storage) {
    sf::Texture texture;
    assert(texture.loadFromFile("resources/sprites/player.png"));
    storage.store("player-sprite", texture);
}

void loadAnimationData(ResourceStorage& storage) {
    using namespace engine::spritesystem;

    sf::Sprite sprite(storage.get<sf::Texture>("player-sprite"));

    LoopingAnimationData playerWalkingSouth {
        sprite,
        {
            {0, 0, 32, 48, 10},
            {32, 0, 32, 48, 10},
            {64, 0, 32, 48, 10},
            {96, 0, 32, 48, 10},
        }
    };

    storage.store("player-walking-south", playerWalkingSouth);
}

void loadSoundEffects(ResourceStorage& storage) {
    static sf::SoundBuffer buffer;
    buffer.loadFromFile("resources/fx/emerald_0005_select_option.wav");

    storage.store("fx-select-option", sf::Sound(buffer));
}

void loadBGM(ResourceStorage& storage) {
    using namespace engine::soundsystem;
    Music littleRoot;
    assert(littleRoot.get().openFromFile("resources/bgm/littleroot-town.wav"));
    storage.store("bgm-littleroot-town", std::move(littleRoot));
}

void loadResources(ResourceStorage& storage) {
    loadFonts(storage);
    loadTextures(storage);
    loadAnimationData(storage);
    loadSoundEffects(storage);
    loadBGM(storage);
}
