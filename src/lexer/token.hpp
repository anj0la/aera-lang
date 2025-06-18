#pragma once

#include <token_type.hpp>
#include <variant>
#include <string>
#include <cstdint>

namespace aera::lexer {

	using Literal = std::variant<std::monostate, int64_t, uint64_t, float, double, std::string, char, bool>

	struct Token {
		TokenType type;
		std::string lexeme;
		Literal literal;
		int line;
		int column;

		Token(TokenType p_type, std::string p_lexeme, Literal p_literal, int p_line, int p_column) : type(p_type), lexeme(p_lexeme), 
			literal(p_literal), line(p_line), column(p_column) {}

		Token(TokenType p_type, std::string p_lexeme, int p_line, int p_column) : type(p_type), lexeme(p_lexeme),
			literal(std::monostate), line(p_line), column(p_column) {
		}
	};
}