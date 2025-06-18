#pragma once

#include <token.hpp>
#include <string>
#include <vector>
#include <unordered_map>

namespace aera::lexer {

	class Lexer {
	public:
		Lexer(std::string p_source);
		~Lexer() = default;

		std::vector<Token> generate_tokens();

	private:
		std::string source;
		std::vector<Token> tokens;
		std::unordered_map<std::string, TokenType> keywords;
	};
}