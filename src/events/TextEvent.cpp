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
    context.priority = 1;
    gameData.inputDispatcher->registerContext("text-state", context);
}

void TextEvent::onStartImpl() {
    sound("fx-select-option", gameData).play();
    enableInputContext("text-state", gameData);
    nextCharIndex = 0;
    timeAccumulator = 0;
    addComponent(map, TextBox{"", content, false}, gameData);
    actionKeyPressed = false;
}

bool TextEvent::tickImpl() {
    TextBox& textBox = data<TextBox>(map, gameData);

    if (textBox.overflow) {
        ECHO("[IMMEDIATE STOP]");
        XTRACE(textBox.content);
        ECHO("Restrict text to:");
        ECHO(textBox.content.substr(0, textBox.content.size() - 1));
        return true;
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
        disableInputContext("text-state", gameData);
        removeComponent<TextBox>(map, gameData);
        return true;
    }

    actionKeyPressed = false;
    return false;
}
