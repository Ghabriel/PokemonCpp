#include "init/load-resources.hpp"

#include <cassert>
#include <fstream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "components/Map.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/resource-system/json/include.hpp"
#include "engine/scripting-system/include.hpp"
#include "engine/sfml/sound-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"
#include "lua-native-functions.hpp"
#include "ResourceFiles.hpp"
#include "TileData.hpp"

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

void loadTiles(ResourceStorage& storage) {
    std::ifstream tilesFile(ResourceFiles::TILES);
    JsonValue data = parseJSON(tilesFile);

    for (const auto& [id, tileData] : data.asIterableMap()) {
        storage.store<TileData>("tile-" + id, {
            tileData["texture"].asString(),
            {
                tileData["rect"][0].asInt(),
                tileData["rect"][1].asInt(),
                tileData["rect"][2].asInt(),
                tileData["rect"][3].asInt()
            }
        });
    }

    ECHO("[RESOURCE] TILES: OK");
}

void loadSequentialTileData(
    JsonValue tileDataArray,
    ResourceStorage& storage,
    Map& map
) {
    for (const auto& tileId : tileDataArray.asIterableArray()) {
        TileData& tileData = storage.get<TileData>("tile-" + tileId.asString());
        Tile tile;
        tile.sprites.emplace_back(storage.get<sf::Texture>(tileData.texture));
        sf::Sprite& layer1 = tile.sprites.back();
        layer1.setTextureRect(tileData.rect);

        map.tiles.push_back(tile);
    }
}

void loadSparseTileData(
    JsonValue tileDataArray,
    ResourceStorage& storage,
    Map& map
) {
    for (const auto& tileCoordinates : tileDataArray.asIterableArray()) {
        int x = tileCoordinates[0].asInt();
        int y = tileCoordinates[1].asInt();
        TileData& tileData = storage.get<TileData>("tile-" + tileCoordinates[2].asString());

        Tile& tile = map.tiles[y * map.widthInTiles + x];
        tile.sprites.emplace_back(storage.get<sf::Texture>(tileData.texture));
        sf::Sprite& layer = tile.sprites.back();
        layer.setTextureRect(tileData.rect);
    }
}

void loadScript(ResourceStorage& storage, const std::string& id) {
    using engine::scriptingsystem::Lua;
    std::string filename = ResourceFiles::SCRIPTS_FOLDER + id + ".lua";
    storage.store(id, Lua(filename));
    injectNativeFunctions(storage.get<Lua>(id));
    ECHO("[RESOURCE] Script '" + id + "': OK");
}

void loadMaps(ResourceStorage& storage) {
    std::ifstream mapsFile(ResourceFiles::MAPS);
    JsonValue data = parseJSON(mapsFile);

    for (const auto& [id, mapData] : data.asIterableMap()) {
        size_t mapId = static_cast<size_t>(mapData["id"].asInt());
        Map map = {
            mapId,
            mapData["name"].asString(),
            static_cast<size_t>(mapData["width-in-tiles"].asInt()),
            static_cast<size_t>(mapData["height-in-tiles"].asInt()),
            {}
        };

        loadSequentialTileData(mapData["layer1"], storage, map);
        loadSparseTileData(mapData["layer2"], storage, map);
        loadScript(storage, "map-" + std::to_string(mapId));

        storage.store(id, map);
    }

    ECHO("[RESOURCE] Maps: OK");
}

void loadResources(ResourceStorage& storage) {
    loadFonts(storage);
    loadTextures(storage);
    loadAnimationData(storage);
    loadSoundEffects(storage);
    loadBGM(storage);
    loadTiles(storage);
    loadMaps(storage);
}
