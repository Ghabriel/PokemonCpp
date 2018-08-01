#ifndef JSON_TOKEN_HPP
#define JSON_TOKEN_HPP

#include <string>

enum class TokenKind {
    EndOfFile,
    StringLiteral,
    NumberLiteral,
    BooleanLiteral,
    NullLiteral,
    OpenBracket,
    CloseBracket,
    OpenBrace,
    CloseBrace,
    Colon,
    Comma,
};

struct Token {
    TokenKind kind;
    std::string text;
};

// std::string tokenKindToString(TokenKind kind) {
//     switch (kind) {
//         case TokenKind::EndOfFile:
//             return "EndOfFile";
//         case TokenKind::StringLiteral:
//             return "StringLiteral";
//         case TokenKind::NumberLiteral:
//             return "NumberLiteral";
//         case TokenKind::BooleanLiteral:
//             return "BooleanLiteral";
//         case TokenKind::NullLiteral:
//             return "NullLiteral";
//         case TokenKind::OpenBracket:
//             return "OpenBracket";
//         case TokenKind::CloseBracket:
//             return "CloseBracket";
//         case TokenKind::OpenBrace:
//             return "OpenBrace";
//         case TokenKind::CloseBrace:
//             return "CloseBrace";
//         case TokenKind::Colon:
//             return "Colon";
//         case TokenKind::Comma:
//             return "Comma";
//     }

//     return "unknown";
// }

// std::string tokenToString(const Token& token) {
//     return tokenKindToString(token.kind) + ": " + token.text;
// }

#endif
