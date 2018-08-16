#ifndef TEXT_BOX_FRAME_HPP
#define TEXT_BOX_FRAME_HPP

#include <optional>

struct TextBoxFrame {
    std::optional<int> fixedWidth;
    int offsetX;
};

#endif
