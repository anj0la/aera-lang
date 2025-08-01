#pragma once

#include <aera/token/token_type.hpp>
#include <aera/common/source_location.hpp>
#include <string>
#include <cstdint>
#include <optional>
#include <variant>

namespace aera {

    using Value = std::variant<int8_t, int16_t, int32_t, int64_t,
        uint8_t, uint16_t, uint32_t, uint64_t,
        float, double, std::string, char, bool>;

    struct Token {

        TokenType type;
        std::string lexeme;
        SourceLocation loc;
        Value value;


        Token(TokenType p_type, const std::string& p_lexeme, const SourceLocation& p_loc, const Value& p_value)
            : type(p_type), lexeme(p_lexeme), loc(p_loc), value(p_value) {
        }

        bool operator==(const Token& other) const {
            return type == other.type &&
                lexeme == other.lexeme &&
                loc == other.loc &&
                value == other.value;
        }
    };
}