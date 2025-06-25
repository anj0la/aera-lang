#pragma once

#include <aera/lexer/token_type.hpp>
#include <variant>
#include <string>
#include <cstdint>

namespace aera::lexer {

    struct Token {
        TokenType type;
        std::string lexeme;
        int line;
        int col;

        Token(TokenType p_type, const std::string& p_lexeme, int p_line, int p_col)
            : type(p_type), lexeme(p_lexeme), line(p_line), col(p_col) {
        }

        bool operator==(const Token& other) const {
            return type == other.type && lexeme == other.lexeme &&
                line == other.line && col == other.col;
        }
    };
}