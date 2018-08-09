#include "events/TextEvent.hpp"

#include "components/TextBox.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

TextEvent::TextEvent(const std::string& content, Entity map, CoreStructures& gameData)
 : content(content), map(map), gameData(gameData) { }

void TextEvent::onStartImpl() {
    nextCharIndex = 0;
    timeAccumulator = 0;
    addComponent(map, TextBox{}, gameData);
}

bool TextEvent::tickImpl() {
    timeAccumulator += *gameData.timeSinceLastFrame;
    if (timeAccumulator >= charDelayMs) {
        data<TextBox>(map, gameData).content += content.at(nextCharIndex);
        ++nextCharIndex;
    }

    // TODO
    return nextCharIndex >= content.size();
}
