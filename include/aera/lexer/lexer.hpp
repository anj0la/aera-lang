#pragma once

#include <aera/token/token.hpp>
#include <aera/diagnostics/diagnostics.hpp>
#include <string>
#include <vector>

namespace aera::lexer {

	class Lexer {
	public:
		Lexer(DiagnosticReporter& reporter, const std::string& filename, const std::string& source);

		// Update the lexer:
		// Pass the DiagnosticsReporter as a variable
		// Create an vector of source lines, read from input in constructorr

		// Whenever an error occures, just add it to the reporter
		// Fix add token to add more information and more importantly, convert the proper types

		~Lexer() = default;

		std::vector<Token> tokenize();
		bool has_error() const;

	private:
		DiagnosticReporter& reporter_;
		std::string filename;
		std::string source;
		std::vector<std::string> source_lines;
		std::vector<Token> tokens;
		bool had_error = false;
		int start = 0;
		int index = 0;
		int line = 1;
		int start_col = 1;
		int col = 1;

		std::vector<std::string> read_lines_from_source();

		SourceLocation current_location() const;
		SourceLocation start_location() const;

		char advance();
		void read_token();
		char peek();
		char peek_next();
		bool is_at_end();
		int current_line() const;
		int current_column() const;

		void add_token(TokenType type, const std::string& lexeme);
		void add_token(TokenType type);
		bool match(char expected);
		void read_punctuation(char c);
		void read_operator(char c);
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