#ifndef INPUT_SYSTEM_KEYBOARD_KEY_HPP
#define INPUT_SYSTEM_KEYBOARD_KEY_HPP

#include <unordered_map>

namespace engine::inputsystem {
    /**
     * \brief Defines all supported keys. Used by RawInput and
     * should be the preferred way to refer to keyboard
     * keys by the game-specific code.
     */
    enum class KeyboardKey {
        Unknown = -1,
        A, B, C, D, E, F, G, H, I, J,
        K, L, M, N, O, P, Q, R, S, T,
        U, V, W, X, Y, Z,
        Num0, Num1, Num2, Num3, Num4,
        Num5, Num6, Num7, Num8, Num9,
        Escape,
        LControl, LShift, LAlt, LSystem,
        RControl, RShift, RAlt, RSystem,
        Menu,
        LBracket, RBracket,
        SemiColon,
        Comma,
        Period,
        Quote,
        Slash,
        BackSlash,
        Tilde,
        Equal,
        Dash,
        Space,
        Return,
        BackSpace,
        Tab,
        PageUp, PageDown,
        End,
        Home,
        Insert,
        Delete,
        Add, Subtract, Multiply, Divide,
        Left, Right, Up, Down,
        Numpad0, Numpad1, Numpad2, Numpad3, Numpad4,
        Numpad5, Numpad6, Numpad7, Numpad8, Numpad9,
        F1, F2, F3, F4, F5,
        F6, F7, F8, F9, F10,
        F11, F12, F13, F14, F15,
        Pause
    };

    inline KeyboardKey keyFromString(const std::string& key) {
        static const std::unordered_map<std::string, KeyboardKey> keyMapping = {
            {"Unknown", KeyboardKey::Unknown},
            {"A", KeyboardKey::A},
            {"B", KeyboardKey::B},
            {"C", KeyboardKey::C},
            {"D", KeyboardKey::D},
            {"E", KeyboardKey::E},
            {"F", KeyboardKey::F},
            {"G", KeyboardKey::G},
            {"H", KeyboardKey::H},
            {"I", KeyboardKey::I},
            {"J", KeyboardKey::J},
            {"K", KeyboardKey::K},
            {"L", KeyboardKey::L},
            {"M", KeyboardKey::M},
            {"N", KeyboardKey::N},
            {"O", KeyboardKey::O},
            {"P", KeyboardKey::P},
            {"Q", KeyboardKey::Q},
            {"R", KeyboardKey::R},
            {"S", KeyboardKey::S},
            {"T", KeyboardKey::T},
            {"U", KeyboardKey::U},
            {"V", KeyboardKey::V},
            {"W", KeyboardKey::W},
            {"X", KeyboardKey::X},
            {"Y", KeyboardKey::Y},
            {"Z", KeyboardKey::Z},
            {"Num0", KeyboardKey::Num0},
            {"Num1", KeyboardKey::Num1},
            {"Num2", KeyboardKey::Num2},
            {"Num3", KeyboardKey::Num3},
            {"Num4", KeyboardKey::Num4},
            {"Num5", KeyboardKey::Num5},
            {"Num6", KeyboardKey::Num6},
            {"Num7", KeyboardKey::Num7},
            {"Num8", KeyboardKey::Num8},
            {"Num9", KeyboardKey::Num9},
            {"Escape", KeyboardKey::Escape},
            {"LControl", KeyboardKey::LControl},
            {"LShift", KeyboardKey::LShift},
            {"LAlt", KeyboardKey::LAlt},
            {"LSystem", KeyboardKey::LSystem},
            {"RControl", KeyboardKey::RControl},
            {"RShift", KeyboardKey::RShift},
            {"RAlt", KeyboardKey::RAlt},
            {"RSystem", KeyboardKey::RSystem},
            {"Menu", KeyboardKey::Menu},
            {"LBracket", KeyboardKey::LBracket},
            {"RBracket", KeyboardKey::RBracket},
            {"SemiColon", KeyboardKey::SemiColon},
            {"Comma", KeyboardKey::Comma},
            {"Period", KeyboardKey::Period},
            {"Quote", KeyboardKey::Quote},
            {"Slash", KeyboardKey::Slash},
            {"BackSlash", KeyboardKey::BackSlash},
            {"Tilde", KeyboardKey::Tilde},
            {"Equal", KeyboardKey::Equal},
            {"Dash", KeyboardKey::Dash},
            {"Space", KeyboardKey::Space},
            {"Return", KeyboardKey::Return},
            {"BackSpace", KeyboardKey::BackSpace},
            {"Tab", KeyboardKey::Tab},
            {"PageUp", KeyboardKey::PageUp},
            {"PageDown", KeyboardKey::PageDown},
            {"End", KeyboardKey::End},
            {"Home", KeyboardKey::Home},
            {"Insert", KeyboardKey::Insert},
            {"Delete", KeyboardKey::Delete},
            {"Add", KeyboardKey::Add},
            {"Subtract", KeyboardKey::Subtract},
            {"Multiply", KeyboardKey::Multiply},
            {"Divide", KeyboardKey::Divide},
            {"Left", KeyboardKey::Left},
            {"Right", KeyboardKey::Right},
            {"Up", KeyboardKey::Up},
            {"Down", KeyboardKey::Down},
            {"Numpad0", KeyboardKey::Numpad0},
            {"Numpad1", KeyboardKey::Numpad1},
            {"Numpad2", KeyboardKey::Numpad2},
            {"Numpad3", KeyboardKey::Numpad3},
            {"Numpad4", KeyboardKey::Numpad4},
            {"Numpad5", KeyboardKey::Numpad5},
            {"Numpad6", KeyboardKey::Numpad6},
            {"Numpad7", KeyboardKey::Numpad7},
            {"Numpad8", KeyboardKey::Numpad8},
            {"Numpad9", KeyboardKey::Numpad9},
            {"F1", KeyboardKey::F1},
            {"F2", KeyboardKey::F2},
            {"F3", KeyboardKey::F3},
            {"F4", KeyboardKey::F4},
            {"F5", KeyboardKey::F5},
            {"F6", KeyboardKey::F6},
            {"F7", KeyboardKey::F7},
            {"F8", KeyboardKey::F8},
            {"F9", KeyboardKey::F9},
            {"F10", KeyboardKey::F10},
            {"F11", KeyboardKey::F11},
            {"F12", KeyboardKey::F12},
            {"F13", KeyboardKey::F13},
            {"F14", KeyboardKey::F14},
            {"F15", KeyboardKey::F15},
            {"Pause", KeyboardKey::Pause}
        };

        return keyMapping.at(key);
    }
}

namespace std {
    template<>
    struct hash<engine::inputsystem::KeyboardKey> {
        size_t operator()(engine::inputsystem::KeyboardKey key) const {
            return std::hash<int>()(static_cast<int>(key));
        }
    };
}

#endif
