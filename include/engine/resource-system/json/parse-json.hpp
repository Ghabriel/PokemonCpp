#ifndef PARSE_JSON_HPP
#define PARSE_JSON_HPP

#include <cassert>
#include <unordered_map>
#include <vector>
#include "JsonValue.hpp"
#include "Scanner.hpp"

namespace __detail {
    JsonValue parseJSON(JsonScanner&);
    JsonValue parseJSON(JsonScanner&, const Token&);
    JsonValue parseArrayLiteral(JsonScanner&);
    JsonValue parseObjectLiteral(JsonScanner&);

    inline void expect(TokenKind kind, const Token& token) {
        assert(token.kind == kind);
    }
}

inline JsonValue parseJSON(std::istream& inputStream) {
    JsonScanner scanner;
    scanner.setInputStream(inputStream);
    return __detail::parseJSON(scanner);
}

namespace __detail {
    inline JsonValue parseJSON(JsonScanner& scanner) {
        return __detail::parseJSON(scanner, scanner.scan());
    }

    inline JsonValue parseJSON(JsonScanner& scanner, const Token& token) {
        switch (token.kind) {
            case TokenKind::StringLiteral:
                return token.text;
            case TokenKind::NumberLiteral:
                return std::stoi(token.text);
            case TokenKind::BooleanLiteral:
                return token.text == "true";
            case TokenKind::NullLiteral:
                return nullptr;
            case TokenKind::OpenBracket:
                return __detail::parseArrayLiteral(scanner);
            case TokenKind::OpenBrace:
                return __detail::parseObjectLiteral(scanner);
            default:
                assert(false);
        }
    }

    inline JsonValue parseArrayLiteral(JsonScanner& scanner) {
        std::vector<JsonValue> result;
        Token token = scanner.scan();

        while (token.kind != TokenKind::CloseBracket) {
            if (result.size() > 0) {
                expect(TokenKind::Comma, token);
                token = scanner.scan();
            }

            result.push_back(parseJSON(scanner, token));
            token = scanner.scan();
        }

        return result;
    }

    inline JsonValue parseObjectLiteral(JsonScanner& scanner) {
        std::unordered_map<std::string, JsonValue> result;
        Token token = scanner.scan();

        while (token.kind != TokenKind::CloseBrace) {
            if (result.size() > 0) {
                expect(TokenKind::Comma, token);
                token = scanner.scan();
            }

            expect(TokenKind::StringLiteral, token);
            expect(TokenKind::Colon, scanner.scan());
            result.insert({token.text, parseJSON(scanner)});
            token = scanner.scan();
        }

        return result;
    }
}

#endif
