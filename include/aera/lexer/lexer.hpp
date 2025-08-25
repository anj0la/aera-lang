#pragma once

#include <aera/token.hpp>
#include <aera/diagnostics.hpp>
#include <aera/source_context.hpp>
#include <string>
#include <vector>
#include <optional>

namespace aera::lexer {

	class Lexer {
	public:
		Lexer(const SourceContext& source_context, DiagnosticReporter& reporter) :
		source_context_(source_context), reporter_(reporter) {}

		~Lexer() = default;

		std::vector<Token> tokenize();

	private:
		const SourceContext& source_context_;
		DiagnosticReporter& reporter_;
		std::string source;
		std::vector<Token> tokens;
		int start = 0;
		int index = 0;
		int line = 1;
		int start_col = 1;
		int col = 1;
		int paren_depth = 0;
		int brace_depth = 0;
		int bracket_depth = 0;

		SourceLocation current_location() const;
		SourceLocation start_location() const;

		char advance();
		void read_token();
		char peek();
		char peek_next();
		Token prev_token();
		bool is_at_end();
		int current_line() const;
		int current_column() const;

		int get_token_length() const;
		void add_token(TokenType type, const std::string& lexeme, const Value& value);
		void add_token(TokenType type);
		bool match(char expected);
		void read_punctuation(char c);
		void read_operator(char c);
		void read_line_comment();
		void read_block_comment();
		void read_character();
		void read_string();
		void read_number(char c);
		void read_hexademical_number();
		void read_binary_number();
		void read_octal_number();
		void read_decimal_number();
		bool is_valid_fractional_part();
		void read_identifier();

		bool is_digit(char c) const;
		bool is_hex_digit(char c) const;
		bool is_binary_digit(char c) const;
		bool is_octal_digit(char c) const;
		bool is_alpha(char c) const;
		bool is_alnum(char c) const;
		bool is_symbol(char c) const;
		bool is_space(char c) const;
		bool is_printable(char c) const;
		void error(const std::string& msg, const std::string& note = "");
	};
}