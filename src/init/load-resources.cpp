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

#include "engine/utils/debug/xtrace.hpp"

using engine::resourcesystem::ResourceStorage;

void loadFonts(ResourceStorage& storage) {
    std::ifstream fontsFile(ResourceFiles::FONTS);
    JsonValue data = parseJSON(fontsFile);

    for (const auto& [id, path] : data.asIterableMap()) {
        sf::Font arial;
        assert(arial.loadFromFile(path.asString()));
        storage.store(id, arial);
    }

    ECHO("[RESOURCE] Fonts: OK");
}

void loadTextures(ResourceStorage& storage) {
    std::ifstream texturesFile(ResourceFiles::TEXTURES);
    JsonValue data = parseJSON(texturesFile);

    for (const auto& [id, path] : data.asIterableMap()) {
        sf::Texture texture;
        assert(texture.loadFromFile(path.asString()));
        storage.store(id, texture);
    }

    ECHO("[RESOURCE] Textures: OK");
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

    ECHO("[RESOURCE] Animations: OK");
}

void loadSoundEffects(ResourceStorage& storage) {
    std::ifstream sfxFile(ResourceFiles::SOUND_EFFECTS);
    JsonValue data = parseJSON(sfxFile);

    for (const auto& [id, path] : data.asIterableMap()) {
        sf::SoundBuffer buffer;
        assert(buffer.loadFromFile(path.asString()));
        storage.store("buffer-" + id, buffer);
        storage.store(id, sf::Sound(storage.get<sf::SoundBuffer>("buffer-" + id)));
    }

    ECHO("[RESOURCE] Sound Effects: OK");
}

void loadBGM(ResourceStorage& storage) {
    std::ifstream bgmFile(ResourceFiles::BGM);
    JsonValue data = parseJSON(bgmFile);

    for (const auto& [id, path] : data.asIterableMap()) {
        engine::soundsystem::Music bgm;
        assert(bgm.get().openFromFile(path.asString()));
        storage.store(id, std::move(bgm));
    }

    ECHO("[RESOURCE] BGM: OK");
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

    ECHO("[RESOURCE] Maps: OK");
}

void loadResources(ResourceStorage& storage) {
    loadFonts(storage);
    loadTextures(storage);
    loadAnimationData(storage);
    loadSoundEffects(storage);
    loadBGM(storage);
    loadMaps(storage);
}
