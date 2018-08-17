#ifndef CORE_FUNCTIONS_HPP
#define CORE_FUNCTIONS_HPP

#include <SFML/Audio.hpp>
#include "engine/entity-system/include.hpp"
#include "engine/input-system/include.hpp"
#include "engine/resource-system/include.hpp"
#include "engine/scripting-system/include.hpp"
#include "engine/sfml/sound-system/include.hpp"
#include "CoreStructures.hpp"
#include "Settings.hpp"

inline Settings& settings(CoreStructures& gameData) {
    return gameData.resourceStorage->get<Settings>("settings");
}

inline engine::entitysystem::Entity createEntity(CoreStructures& gameData) {
    return gameData.componentManager->createEntity();
}

inline void deleteEntity(engine::entitysystem::Entity entity, CoreStructures& gameData) {
    gameData.componentManager->deleteEntity(entity);
}

inline void restoreEntity(engine::entitysystem::Entity entity, CoreStructures& gameData) {
    gameData.componentManager->restoreEntity(entity);
}

template<typename TComponent>
inline void addComponent(
    engine::entitysystem::Entity entity,
    TComponent&& data,
    CoreStructures& gameData
) {
    gameData.componentManager->addComponent(entity, std::forward<TComponent>(data));
}

template<typename TComponent>
inline void removeComponent(engine::entitysystem::Entity entity, CoreStructures& gameData) {
    gameData.componentManager->removeComponent<TComponent>(entity);
}

template<typename TComponent>
inline bool hasComponent(engine::entitysystem::Entity entity, CoreStructures& gameData) {
    return gameData.componentManager->hasComponent<TComponent>(entity);
}

template<typename TComponent>
inline TComponent& data(engine::entitysystem::Entity entity, CoreStructures& gameData) {
    return gameData.componentManager->getData<TComponent>(entity);
}

inline void enableInputContext(const std::string& id, CoreStructures& gameData) {
    gameData.inputDispatcher->enableContext(id);
}

inline void disableInputContext(const std::string& id, CoreStructures& gameData) {
    gameData.inputDispatcher->disableContext(id);
}

template<typename TResource>
inline TResource& resource(const std::string& id, CoreStructures& gameData) {
    return gameData.resourceStorage->get<TResource>(id);
}

inline engine::scriptingsystem::Lua& script(const std::string& id, CoreStructures& gameData) {
    using engine::scriptingsystem::Lua;
    return gameData.resourceStorage->get<Lua>(id);
}

inline sf::Music& music(const std::string& id, CoreStructures& gameData) {
    using engine::soundsystem::Music;
    return gameData.resourceStorage->get<Music>(id).get();
}

inline sf::Sound& sound(const std::string& id, CoreStructures& gameData) {
    return gameData.resourceStorage->get<sf::Sound>(id);
}

#endif
