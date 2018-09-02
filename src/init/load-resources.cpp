#include "init/load-resources.hpp"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <SFML/Audio.hpp>
#include <SFML/Graphics.hpp>
#include "battle/data/EncounterData.hpp"
#include "battle/data/Move.hpp"
#include "battle/data/PokemonSpeciesData.hpp"
#include "battle/helpers/move-effects.hpp"
#include "components/Map.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/resource-system/json/include.hpp"
#include "engine/scripting-system/include.hpp"
#include "engine/sfml/sound-system/include.hpp"
#include "engine/sfml/sprite-system/include.hpp"
#include "lua-native-functions.hpp"
#include "ResourceFiles.hpp"
#include "Settings.hpp"
#include "TileData.hpp"

#include "engine/utils/debug/xtrace.hpp"

using engine::resourcesystem::ResourceStorage;

void loadFonts(ResourceStorage& storage) {
    std::ifstream fontsFile(ResourceFiles::FONTS);
    JsonValue data = parseJSON(fontsFile);

    for (const auto& [id, path] : data.asIterableMap()) {
        sf::Font font;
        assert(font.loadFromFile(path.asString()));
        storage.store(id, font);
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

    for (const auto& [id, bgmData] : data.asIterableMap()) {
        engine::soundsystem::Music bgmWrapper;
        sf::Music& bgm = bgmWrapper.get();
        auto bgmSettings = bgmData.get<std::unordered_map<std::string, JsonValue>>();
        assert(bgm.openFromFile(bgmSettings["file"].asString()));

        float loopStart = 0;
        if (bgmSettings.count("loop-start")) {
            loopStart = std::stof(bgmSettings["loop-start"].asString());
        }

        float loopEnd = bgm.getDuration().asSeconds();
        if (bgmSettings.count("loop-end")) {
            loopEnd = std::stof(bgmSettings["loop-end"].asString());
        }

        bgm.setLoopPoints({sf::seconds(loopStart), sf::seconds(loopEnd - loopStart)});

        if (bgmSettings.count("start-offset")) {
            float startOffset = std::stof(bgmSettings["start-offset"].asString());
            bgm.setPlayingOffset(sf::seconds(startOffset));
        }

        if (bgmSettings.count("volume")) {
            float volume = std::stof(bgmSettings["volume"].asString());
            bgm.setVolume(volume);
        }

        bgm.setLoop(true);
        storage.store(id, std::move(bgmWrapper));
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

void loadEncounters(ResourceStorage& storage) {
    std::ifstream encountersFile(ResourceFiles::ENCOUNTERS);
    JsonValue data = parseJSON(encountersFile);

    for (const auto& [mapId, mapEncounterData] : data.asIterableMap()) {
        MapEncounterData mapEncounters;

        for (const auto& [environment, encounterDataArray] : mapEncounterData.asIterableMap()) {
            std::vector<EncounterData> encounterList;

            for (const auto& encounterData : encounterDataArray.asIterableArray()) {
                EncounterData encounter;
                encounter.pokemon = encounterData["pokemon"].asString();
                encounter.minLevel = encounterData["min-level"].asInt();
                encounter.maxLevel = encounterData["max-level"].asInt();
                encounter.rate = encounterData["rate"].asInt();
                encounterList.push_back(encounter);
            }

            mapEncounters.encounterData[environment] = encounterList;
        }

        storage.store("encounters-" + mapId, mapEncounters);
    }

    ECHO("[RESOURCE] Encounters: OK");
}

std::vector<std::string> asStringVector(const JsonValue& value) {
    std::vector<std::string> result;

    for (const auto& member : value.asIterableArray()) {
        result.push_back(member.asString());
    }

    return result;
}

std::array<int, 6> asStatArray(const JsonValue& value) {
    std::array<int, 6> result;
    size_t i = 0;

    for (const auto& member : value.asIterableArray()) {
        result[i] = member.asInt();
        ++i;
    }

    return result;
}

float asFloat(const JsonValue& value) {
    return std::stof(value.asString());
}

std::vector<std::pair<int, std::string>> asMoveList(const JsonValue& value) {
    std::vector<std::pair<int, std::string>> result;

    for (const auto& entry : value.asIterableArray()) {
        result.push_back({
            entry["level"].asInt(),
            entry["move"].asString()
        });
    }

    return result;
}

std::vector<EvolutionData> asEvolutionData(const JsonValue& value) {
    std::vector<EvolutionData> result;

    for (const auto& entry : value.asIterableArray()) {
        result.push_back({
            entry["pokemon"].asString(),
            entry["method"]
        });
    }

    return result;
}

std::vector<std::string> loadPokemonSpecies(ResourceStorage& storage) {
    std::ifstream pokemonFile(ResourceFiles::POKEMON);
    JsonValue data = parseJSON(pokemonFile);
    std::vector<std::string> pokemonList;

    for (const auto& [id, pokemonData] : data.asIterableMap()) {
        PokemonSpeciesData species;
        species.displayName = pokemonData["display-name"].asString();
        species.nationalNumber = pokemonData["national-number"].asInt();
        species.types = asStringVector(pokemonData["types"]);
        species.baseStats = asStatArray(pokemonData["base-stats"]);
        species.maleRatio = pokemonData["male-ratio"].asString();
        species.growthRate = pokemonData["growth-rate"].asString();
        species.baseExp = pokemonData["base-exp"].asInt();
        species.effortPoints = asStatArray(pokemonData["effort-points"]);
        species.captureRate = pokemonData["capture-rate"].asInt();
        species.baseHappiness = pokemonData["base-happiness"].asInt();
        species.abilities = asStringVector(pokemonData["abilities"]);
        species.hiddenAbilities = asStringVector(pokemonData["hidden-abilities"]);
        species.moves = asMoveList(pokemonData["moves"]);
        species.eggMoves = asStringVector(pokemonData["egg-moves"]);
        species.eggGroups = asStringVector(pokemonData["egg-groups"]);
        species.eggSteps = pokemonData["egg-steps"].asInt();
        species.height = asFloat(pokemonData["height"]);
        species.weight = asFloat(pokemonData["weight"]);
        species.color = pokemonData["color"].asString();
        species.shape = pokemonData["shape"].asInt();
        species.habitat = pokemonData["habitat"].asString();
        species.kind = pokemonData["kind"].asString();
        species.pokedexDescription = pokemonData["pokedex-description"].asString();
        species.battlePlayerY = pokemonData["battle-player-y"].asInt();
        species.battleEnemyY = pokemonData["battle-enemy-y"].asInt();
        species.battleAltitude = pokemonData["battle-altitude"].asInt();
        species.evolutions = asEvolutionData(pokemonData["evolutions"]);

        storage.store("pokemon-" + id, species);
        pokemonList.push_back(id);
    }

    ECHO("[RESOURCE] Pokemon: OK");
    return pokemonList;
}

void loadPokemonSprites(
    ResourceStorage& storage,
    const std::vector<std::string>& pokemonList
) {
    Settings& settings = storage.get<Settings>("settings");
    std::string backSprites = settings.getPokemonBackSpritesFolder();
    std::string frontSprites = settings.getPokemonFrontSpritesFolder();

    for (const std::string& id : pokemonList) {
        std::string lowercaseId = id;
        std::transform(id.begin(), id.end(), lowercaseId.begin(), tolower);

        sf::Texture backTexture;
        assert(backTexture.loadFromFile(backSprites + lowercaseId + ".png"));
        storage.store("pokemon-back-" + id, backTexture);

        sf::Texture frontTexture;
        assert(frontTexture.loadFromFile(frontSprites + lowercaseId + ".png"));
        storage.store("pokemon-front-" + id, frontTexture);
    }

    ECHO("[RESOURCE] Pokemon sprites: OK");
}

void loadMoves(ResourceStorage& storage) {
    std::ifstream movesFile(ResourceFiles::MOVES);
    JsonValue data = parseJSON(movesFile);

    for (const auto& [id, moveData] : data.asIterableMap()) {
        Move move;
        move.id = id;
        move.displayName = moveData["display-name"].asString();
        move.type = moveData["type"].asString();
        move.kind = moveData["kind"].asString();
        move.functionCode = moveData["function-code"].asInt();
        move.functionParameter = moveData["function-parameter"].asInt();
        move.power = moveData["power"].asInt();
        move.accuracy = moveData["accuracy"].asInt();
        move.pp = moveData["pp"].asInt();
        move.effectRate = moveData["effect-rate"].asInt();
        move.targetType = moveData["target-type"].asString();
        move.priority = moveData["priority"].asInt();
        move.flags = moveData["flags"].asString();
        move.description = moveData["description"].asString();

        storage.store("move-" + id, move);
    }

    ECHO("[RESOURCE] Moves: OK");
}

void loadBattleScripts(ResourceStorage& storage) {
    using engine::scriptingsystem::Lua;
    loadScript(storage, "ai");
    loadScript(storage, "moves");
    injectNativeBattleFunctions(storage.get<Lua>("moves"));
    ECHO("[RESOURCE] Battle scripts: OK");
}

void loadResources(ResourceStorage& storage) {
    loadFonts(storage);
    loadTextures(storage);
    loadAnimationData(storage);
    loadSoundEffects(storage);
    loadBGM(storage);
    loadTiles(storage);
    loadMaps(storage);
    loadEncounters(storage);
    std::vector<std::string> pokemonList = loadPokemonSpecies(storage);
    loadPokemonSprites(storage, pokemonList);
    loadMoves(storage);
    loadBattleScripts(storage);
}
