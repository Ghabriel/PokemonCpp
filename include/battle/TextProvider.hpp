#ifndef TEXT_PROVIDER_HPP
#define TEXT_PROVIDER_HPP

#include <string>
#include "../engine/entity-system/types.hpp"

struct CoreStructures;

class TextProvider {
    using Entity = engine::entitysystem::Entity;
 public:
    virtual ~TextProvider() = default;

    void showBattleText(
        const std::string& content,
        Entity battleEntity,
        CoreStructures& gameData
    ) {
        showBattleTextImpl(content, battleEntity, gameData);
    }

    void showMoveText(
        const std::string& content,
        Entity battleEntity,
        CoreStructures& gameData
    ) {
        showMoveTextImpl(content, battleEntity, gameData);
    }

 private:
    virtual void showBattleTextImpl(const std::string&, Entity, CoreStructures&) = 0;
    virtual void showMoveTextImpl(const std::string&, Entity, CoreStructures&) = 0;
};

#endif
