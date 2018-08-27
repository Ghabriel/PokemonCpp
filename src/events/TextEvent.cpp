#include "events/TextEvent.hpp"

#include "components/TextBox.hpp"
#include "core-functions.hpp"
#include "CoreStructures.hpp"
#include "engine/input-system/include.hpp"

using engine::inputsystem::InputContext;

bool actionKeyPressed;

TextEvent::TextEvent(const std::string& content, Entity map, CoreStructures& gameData)
 : content(content), map(map), gameData(gameData) {
    InputContext context;
    context.actions = {{"Action", [&] { actionKeyPressed = true; }}};
    context.priority = 10;
    gameData.inputDispatcher->registerContext("text-state", context);
}

void TextEvent::onStartImpl() {
    // sound("fx-select-option", gameData).play();
    enableInputContext("disabled-controls", gameData);
    enableInputContext("text-state", gameData);
    nextCharIndex = 0;
    timeAccumulator = 0;
    addComponent(map, TextBox{"", content, false}, gameData);
    actionKeyPressed = false;
}

bool TextEvent::tickImpl() {
    TextBox& textBox = data<TextBox>(map, gameData);

    if (textBox.overflow) {
        if (actionKeyPressed) {
            content = textBox.fullContent.substr(textBox.content.size() - 1);
            onClose();
            onStart();
            return tick();
        }

        return false;
    }

    if (nextCharIndex < content.size()) {
        timeAccumulator += *gameData.timeSinceLastFrame;
        if (timeAccumulator >= charDelayMs) {
            textBox.content += content.at(nextCharIndex);
            ++nextCharIndex;
            timeAccumulator = 0;
        }

        actionKeyPressed = false;
        return false;
    }

    if (actionKeyPressed) {
        sound("fx-select-option", gameData).play();
        onClose();
        return true;
    }

    actionKeyPressed = false;
    return false;
}

void TextEvent::onClose() {
    disableInputContext("text-state", gameData);
    disableInputContext("disabled-controls", gameData);
    removeComponent<TextBox>(map, gameData);
}
