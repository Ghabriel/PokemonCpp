#ifndef TEXT_BOX_HPP
#define TEXT_BOX_HPP

#include <string>

struct TextBox {
    std::string content;
    std::string fullContent;
    bool overflow;
    std::string cachedParsedText;
    size_t cachedContentSize;
};

#endif
