#include "init/load-resources.hpp"

#include <cassert>
#include <fstream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "components/Map.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/resource-system/json/include.hpp"
#include "engine/sfml/sound-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"
#include "ResourceFiles.hpp"

using engine::resourcesystem::ResourceStorage;

void loadFonts(ResourceStorage& storage) {
    sf::Font arial;
    assert(arial.loadFromFile("resources/fonts/arial.ttf"));
    storage.store("font-arial", arial);
}

void loadTextures(ResourceStorage& storage) {
    std::ifstream texturesFile(ResourceFiles::TEXTURES);
    JsonValue data = parseJSON(texturesFile);

    for (const auto& [id, path] : data.asIterableMap()) {
        sf::Texture texture;
        assert(texture.loadFromFile(path.asString()));
        storage.store(id, texture);
    }
}

void loadAnimationData(ResourceStorage& storage) {
    using namespace engine::spritesystem;

    std::ifstream animationsFile(ResourceFiles::ANIMATIONS);
    JsonValue data = parseJSON(animationsFile);

    for (const auto& [id, animationData] : data.asIterableMap()) {
        sf::Sprite sprite(
            storage.get<sf::Texture>(
                animationData["texture"].asString()
            )
        );

        const JsonValue& jsonFrames = animationData["frames"];
        std::vector<Frame> frames;

        for (const auto& frameData : jsonFrames.asIterableArray()) {
            frames.push_back({
                frameData[0].asInt(),
                frameData[1].asInt(),
                frameData[2].asInt(),
                frameData[3].asInt(),
                frameData[4].asInt()
            });
        }

        storage.store(id, LoopingAnimationData { sprite, frames });
    }
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

void loadMaps(ResourceStorage& storage) {
    using namespace engine::spritesystem;

    sf::Sprite terrain(storage.get<sf::Texture>("terrain-sprite"));

    constexpr int widthInTiles = 25;
    constexpr int heightInTiles = 20;

    Map basicMap = {1, "Test Map", widthInTiles, heightInTiles, {}};
    Tile simpleTile { terrain };
    simpleTile.sprite.setTextureRect({0, 0, 32, 32});

    for (size_t i = 0; i < widthInTiles * heightInTiles; ++i) {
        basicMap.tiles.push_back(simpleTile);
    }

    storage.store("map-basic", basicMap);
}

void loadResources(ResourceStorage& storage) {
    loadFonts(storage);
    loadTextures(storage);
    loadAnimationData(storage);
    loadSoundEffects(storage);
    loadBGM(storage);
    loadMaps(storage);
}
