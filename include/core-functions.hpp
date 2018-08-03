#ifndef CORE_FUNCTIONS_HPP
#define CORE_FUNCTIONS_HPP

#include <SFML/Audio.hpp>
#include "engine/entity-system/include.hpp"
#include "engine/input-system/include.hpp"
#include "engine/sfml/sound-system/include.hpp"
#include "CoreStructures.hpp"

template<typename TComponent>
void addComponent(
    engine::entitysystem::Entity entity,
    TComponent&& data,
    CoreStructures& gameData
) {
    gameData.componentManager->addComponent(entity, std::forward<TComponent>(data));
}

template<typename TComponent>
void removeComponent(engine::entitysystem::Entity entity, CoreStructures& gameData) {
    gameData.componentManager->removeComponent<TComponent>(entity);
}

template<typename TComponent>
bool hasComponent(engine::entitysystem::Entity entity, CoreStructures& gameData) {
    return gameData.componentManager->hasComponent<TComponent>(entity);
}

template<typename TComponent>
TComponent& data(engine::entitysystem::Entity entity, CoreStructures& gameData) {
    return gameData.componentManager->getData<TComponent>(entity);
}

void enableInputContext(const std::string& id, CoreStructures& gameData) {
    gameData.inputDispatcher->enableContext(id);
}

void disableInputContext(const std::string& id, CoreStructures& gameData) {
    gameData.inputDispatcher->disableContext(id);
}

sf::Music& music(const std::string& id, CoreStructures& gameData) {
    using engine::soundsystem::Music;
    return gameData.resourceStorage->get<Music>(id).get();
}

sf::Sound& sound(const std::string& id, CoreStructures& gameData) {
    return gameData.resourceStorage->get<sf::Sound>(id);
}

#endif
