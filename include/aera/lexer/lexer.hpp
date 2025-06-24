#pragma once

#include <aera/lexer/token.hpp>

#include <string>
#include <vector>

namespace aera::lexer {

	class Lexer {
	public:
		Lexer(std::string p_input);
		~Lexer() = default;

		std::vector<Token> tokenize();
		char advance();
		void read_token();
		char peek();
		char peek_next();
		bool is_at_end();
		void reset();
		void set_input(const std::string& string);
		int current_line() const;
		int current_column() const;

		bool has_error() const;

	private:
		std::string input;
		std::vector<Token> tokens;
		bool had_error = false;
		int start = 0;
		int index = 0;
		int line = 1;
		int column = 1;

		void add_token(TokenType type, Literal literal);
		void add_token(TokenType type);
		bool match(char expected);
		void read_line_comment();
		void read_block_comment();
		void read_character();
		void read_string();
		void read_number();
		void read_identifier();
		
		bool is_digit(char c) const;
		bool is_alpha(char c) const;
		bool is_alnum(char c) const;
		bool is_symbol(char c) const;
		bool is_space(char c) const;

		void error(const std::string& message);
	};
}