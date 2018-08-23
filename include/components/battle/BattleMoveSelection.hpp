#ifndef BATTLE_MOVE_SELECTION_HPP
#define BATTLE_MOVE_SELECTION_HPP

#include <array>
#include <optional>
#include <string>

struct MoveDisplayInfo {
    std::string displayName;
    std::string type;
    int pp;
    int maxPP;
};

struct BattleMoveSelection {
    std::array<std::optional<MoveDisplayInfo>, 4> moves;
    int optionBoxWidth;
    size_t focusedOption;
};

#endif
