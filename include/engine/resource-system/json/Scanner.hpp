#ifndef JSON_SCANNER_HPP
#define JSON_SCANNER_HPP

#include <cassert>
#include <istream>
#include <sstream>
#include <stdexcept>
#include <string>
#include "Token.hpp"

class JsonScanner {
 public:
    void setInputStream(std::istream&);
    bool eof() const;
    Token scan();

 private:
    std::istream* stream;
    char currentChar = 0;

    void skipComment();
    Token scanStringLiteral();
    Token scanNumberLiteral(char firstChar);
    Token scanExactly(const std::string& content, TokenKind);
};

inline void JsonScanner::setInputStream(std::istream& inputStream) {
    stream = &inputStream;
    currentChar = 0;
}

inline bool JsonScanner::eof() const {
    return stream->eof();
}

inline Token JsonScanner::scan() {
    char ch = currentChar == 0 ? stream->get() : currentChar;
    currentChar = 0;
    constexpr auto eof = std::char_traits<char>::eof();

    switch (ch) {
        case eof:
            return {TokenKind::EndOfFile, ""};
        case ' ':
        case '\t':
        case '\n':
            return scan();
        case '/':
            skipComment();
            return scan();
        case '"':
            return scanStringLiteral();
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '-':
            return scanNumberLiteral(ch);
        case 't':
            return scanExactly("true", TokenKind::BooleanLiteral);
        case 'f':
            return scanExactly("false", TokenKind::BooleanLiteral);
        case 'n':
            return scanExactly("null", TokenKind::NullLiteral);
        case '[':
            return {TokenKind::OpenBracket, "["};
        case ']':
            return {TokenKind::CloseBracket, "]"};
        case '{':
            return {TokenKind::OpenBrace, "{"};
        case '}':
            return {TokenKind::CloseBrace, "}"};
        case ':':
            return {TokenKind::Colon, ":"};
        case ',':
            return {TokenKind::Comma, ","};
        default:
            throw std::runtime_error("Invalid character: " + std::to_string(ch));
    }
}

inline void JsonScanner::skipComment() {
    char ch = stream->get();

    if (ch == '/') {
        while (stream->get() != '\n');
        return;
    }

    if (ch == '*') {
        while (stream->get() != '*' || stream->get() != '/');
        return;
    }

    assert(false);
}

inline Token JsonScanner::scanStringLiteral() {
    std::stringstream ss;
    char ch = stream->get();

    while (ch != '"') {
        ss << ch;
        ch = stream->get();
    }

    return {TokenKind::StringLiteral, ss.str()};
}

inline Token JsonScanner::scanNumberLiteral(char ch) {
    std::stringstream ss;

    do {
        ss << ch;
        ch = stream->get();
    } while (ch >= '0' && ch <= '9');

    currentChar = ch;
    return {TokenKind::NumberLiteral, ss.str()};
}

inline Token JsonScanner::scanExactly(const std::string& content, TokenKind tokenKind) {
    for (size_t i = 1; i < content.size(); ++i) {
        char ch = stream->get();

        if (ch != content[i]) {
            throw std::runtime_error("Invalid character: " + std::to_string(ch));
        }
    }

    return {tokenKind, content};
}

#endif
