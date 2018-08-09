#include "events/TextEvent.hpp"

#include "components/TextBox.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"

TextEvent::TextEvent(const std::string& content, Entity map, CoreStructures& gameData)
 : content(content), map(map), gameData(gameData) { }

void TextEvent::onStartImpl() {
    nextChar = 0;
    accumulator = 0;
    addComponent(map, TextBox{}, gameData);
}

bool TextEvent::tickImpl() {
    // TODO
    return true;
}
