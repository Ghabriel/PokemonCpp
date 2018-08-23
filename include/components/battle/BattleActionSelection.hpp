#ifndef BATTLE_ACTION_SELECTION_HPP
#define BATTLE_ACTION_SELECTION_HPP

#include <array>
#include <string>

struct BattleActionSelection {
    std::string text;
    std::array<std::string, 4> options;
    int optionBoxWidth;
    size_t focusedOption;
};

#endif
