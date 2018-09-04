#ifndef NULL_TEXT_PROVIDER_HPP
#define NULL_TEXT_PROVIDER_HPP

#include "../TextProvider.hpp"

class NullTextProvider : public TextProvider {
    using Entity = engine::entitysystem::Entity;
 private:
    void showBattleTextImpl(
        const std::string& content,
        Entity battleEntity,
        CoreStructures& gameData
    ) override { }

    void showMoveTextImpl(
        const std::string& content,
        Entity battleEntity,
        CoreStructures& gameData
    ) override { }
};

#endif
