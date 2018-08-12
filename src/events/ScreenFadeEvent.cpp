#include "events/ScreenFadeEvent.hpp"

#include "components/Camera.hpp"
#include "components/DrawableVector.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "lua-native-functions.hpp"
#include "engine/utils/debug/xtrace.hpp"

ScreenFadeEvent::ScreenFadeEvent(Entity map, CoreStructures& gameData)
 : map(map), gameData(gameData) { }

void ScreenFadeEvent::onStartImpl() {
    Camera& camera = resource<Camera>("camera", gameData);
    float cameraWidth = camera.width;
    topRect = sf::RectangleShape({cameraWidth, 0});
    topRect.setPosition(0, 0);
    topRect.setFillColor(sf::Color::Black);
    bottomRect = sf::RectangleShape({cameraWidth, 0});
    bottomRect.setPosition(0, camera.height);
    bottomRect.setFillColor(sf::Color::Black);
    addComponent(map, DrawableVector{{&topRect, &bottomRect}}, gameData);

    music("bgm-littleroot-town", gameData).stop();
    sf::Music& bgm = music("bgm-wild-battle", gameData);
    bgm.setLoop(true);
    float loopStart = 16.8;
    float loopEnd = 175;
    bgm.setLoopPoints({sf::seconds(loopStart), sf::seconds(loopEnd - loopStart)});
    bgm.setPlayingOffset(sf::seconds(0.8));
    bgm.setVolume(30);
    bgm.play();
}

bool ScreenFadeEvent::tickImpl() {
    Camera& camera = resource<Camera>("camera", gameData);
    sf::Vector2f rectSize = topRect.getSize();
    rectSize.y += camera.height / 120;

    topRect.setSize(rectSize);
    bottomRect.setSize(rectSize);
    bottomRect.setPosition(0, camera.height - rectSize.y);

    if (rectSize.y >= camera.height / 2) {
        lua::enableControls();
        return true;
    }

    return false;
}
