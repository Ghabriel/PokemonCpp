#ifndef TESTING_COLOR_HPP
#define TESTING_COLOR_HPP

#include <sstream>

enum class FontDecoration {
    None = 0,
    Bold = 1,
    // Background = 3,
    Underline = 4
};

enum class FontColor {
    Reset = 0,
    Gray = 30,
    Red = 31,
    Green = 32,
    Yellow = 33,
    Blue = 34,
    Purple = 35,
    Cyan = 36,
    White = 37
};

inline std::string setColor(
    FontColor color,
    FontDecoration decoration = FontDecoration::None
) {
    std::stringstream ss;
    ss << "\x1b[";
    ss << static_cast<int>(color);

    if (decoration != FontDecoration::None) {
        ss << ';';
        ss << static_cast<int>(decoration);
    }

    ss << "m";
    return ss.str();
}

inline std::string resetColor() {
    return setColor(FontColor::Reset);
}

inline std::string applyColor(
    const std::string& text,
    FontColor color,
    FontDecoration decoration = FontDecoration::None
) {
    return setColor(color, decoration) + text + resetColor();
}

#endif
