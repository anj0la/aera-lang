#pragma once

#include <aera/lexer/token_type.hpp>
#include <variant>
#include <string>
#include <cstdint>

namespace aera::lexer {

	using Literal = std::variant<std::string, char, bool>;

    struct Token {
        TokenType type;
        std::string lexeme;
        Literal literal;
        int line;
        int column;

        // Constructor with literal

        Token(TokenType p_type, const std::string& p_lexeme, const Literal& p_literal,
            int p_line, int p_column)
            : type(p_type), lexeme(p_lexeme), literal(p_literal), line(p_line), column(p_column) {
        }

        // Default constructor (EOF)

        Token() : type(TokenType::Eof), lexeme(""), literal(""), line(0), column(0) {}

        bool operator==(const Token& other) const {
            return type == other.type && lexeme == other.lexeme &&
                literal == other.literal && line == other.line && column == other.column;
        }
    };
}