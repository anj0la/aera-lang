#include <aera/lexer/lexer.hpp>
#include <iostream>

namespace aera::lexer {

	Lexer::Lexer(std::string p_input) : input(p_input) {}

	std::vector<Token> Lexer::tokenize() {

		while (!is_at_end()) {
			start = index;
			start_col = col;
			read_token();
		}

		tokens.push_back(Token(TokenType::Eof, "", line, col));
		return tokens;
	}

	bool Lexer::has_error() const {
		return had_error;
	}

	char Lexer::advance() {
		char ch = input[index];

		switch (ch) {
			case '\n':
				line++;
				col = 1;
				break;
			case '\t':
				col += 4;
				break;
			case '\r':
				col = 1;
				break;
			default:
				col++;
				break;
			}
		
		index++;
		return ch;
	}

	void Lexer::read_token() {
		char ch = advance();
		
		switch (ch) {

			// Punctuation

			case '(': add_token(TokenType::LeftParen); break;
			case ')': add_token(TokenType::RightParen); break;
			case '{': add_token(TokenType::LeftBrace); break;
			case '}': add_token(TokenType::RightBrace); break;
			case '[': add_token(TokenType::LeftBracket); break;
			case ']': add_token(TokenType::RightBracket); break;
			case ',': add_token(TokenType::Comma); break;
			case '.':
				if (match('.')) {
					if (match('=')) {
						add_token(TokenType::RangeInclusiveOp); // ..=
					}
					else {
						add_token(TokenType::RangeExclusiveOp); // ..
					}
				}
				else {
					add_token(TokenType::Period);
				}
				break;
			case ';': add_token(TokenType::Semicolon); break;
			case ':': add_token(TokenType::Colon); break;

			// Potential operators

			case '?': add_token(TokenType::Question); break;
			case '@': add_token(TokenType::At); break;

			// Arithmetic operators

			case '+': add_token(TokenType::PlusOp); break;

			// Special case -> the minus operator could lead to the arrow (or return type) operator
			case '-': 
				add_token((match('>') ? TokenType::ArrowOp : TokenType::MinusOp));
				break;

			case '*': add_token(TokenType::StarOp); break;
			case '/': add_token(TokenType::SlashOp); break;
			case '%': add_token(TokenType::PercentOp); break;

			// Comparison operators

			case '!':
				add_token((match('=') ? TokenType::ExclaimEqualOp : TokenType::ExclaimOp));
				break;

			case '=':
				add_token((match('=') ? TokenType::EqualOp : TokenType::AssignOp));
				break;

			case '>':
				add_token((match('=') ? TokenType::GreaterEqualOp : TokenType::GreaterOp));
				break;

			// Special case -> check for block comment starting with <#
			case '<':
				if (match('#')) {
					read_block_comment();
				}
				else { // Otherwise, add as usual
					add_token((match('=') ? TokenType::LessEqualOp : TokenType::LessOp));
				}
				break;

			// Bitwise / Logical operators

			case '&':
				if (match('&')) {
					add_token(TokenType::LogicalAndOp);  // &&
				}
				else {
					add_token(TokenType::BitwiseAndOp);   // &
				}
				break;

			case '|':
				if (match('|')) {
					add_token(TokenType::LogicalOrOp);   // ||
				}
				else {
					add_token(TokenType::BitwiseOrOp);        // |
				}
				break;

			// Line comments

			case '#':
				read_line_comment();
				break;

			// Ignore spaces, new line and carriage return

			case ' ':
			case '\r':
			case '\t':
			case '\n':
				break;

			// Character literal

			case '\'': read_character(); break; // characters always start with '

			// String literal

			case '"': read_string(); break; // strings always start with "

			// Integer, float and identifiers are handlded in the default case

			default:
				if (is_digit(ch)) {
					read_number(); // handles integer and floating point literals
				}
				else if (is_alpha(ch)) {
					read_identifier();
				}
				else {
					had_error = true;
					std::string bad_char(1, ch);
					add_token(TokenType::Illegal, bad_char); // unexpected character
				}
				break;
		}
	}

	char Lexer::peek() {
		if (is_at_end()) {
			return '\0';
		}

		return input[index];
	}

	char Lexer::peek_next() {
		if (index + 1 >= input.length()) {
			return '\0';
		}

		return input[index + 1];
	}

	bool Lexer::is_at_end() {
		return index >= input.length();
	}

	int Lexer::current_line() const {
		return line;
	}

	int Lexer::current_column() const {
		return col;
	}

	void Lexer::add_token(TokenType type, const std::string& lexeme) {
		tokens.push_back(Token(type, lexeme, line, start_col));
	}

	void Lexer::add_token(TokenType type) {
		std::string text = input.substr(start, index - start);
		tokens.push_back(Token(type, text, line, start_col));
	}

	bool Lexer::match(char expected) {
		if (is_at_end()) {
			return false;
		}

		if (input[index] != expected) {
			return false;
		}

		advance(); // to handle line and col
		return true;
	}

	void Lexer::read_line_comment() {
		while (peek() != '\n' && !is_at_end()) {
			advance();
		}
	}

	void Lexer::read_block_comment() {
		while (true) {
			if (is_at_end()) {
				had_error = true;
				add_token(TokenType::Illegal, "Unterminated block comment.");
				break;
			}
			if (peek() == '\n') {
				had_error = true;
				add_token(TokenType::Illegal, "Unterminated block comment.");
				break;
			}
			if (peek() == '#' && peek_next() == '>') {
				advance(); // consume '#'
				advance(); // consume '>'
				break; 
			}
			advance();
		}
	}

	void Lexer::read_character() {
		if (peek() == '\'') {
			advance(); // Consume the '
			had_error = true;
			add_token(TokenType::Illegal, "Empty character literal.");
			return;
		}

		char ch = advance(); // Consume char

		if (ch == '\\' && !is_at_end()) { // Handle escape sequences
			char escaped = advance();
			switch (escaped) {
			case 'n': ch = '\n'; break;
			case 't': ch = '\t'; break;
			case 'r': ch = '\r'; break;
			case '\\': ch = '\\'; break;
			case '\'': ch = '\''; break;
			case '"': ch = '"'; break;
			default:
				had_error = true;
				add_token(TokenType::Illegal, "Invalid escape sequence: \\" + std::string(1, escaped));
				break;
			}
		}
		else if (!(is_alpha(ch) || is_digit(ch) || is_symbol(ch) || is_space(ch))) {
			if (ch == '\\') {
				had_error = true;
				add_token(TokenType::Illegal, "Unterminated escape sequence in character literal.");
			}
			else {
				had_error = true;
				add_token(TokenType::Illegal, "Invalid character in literal: " + std::string(1, ch));
			}
		}

		if (peek() != '\'') {
			if (is_at_end()) {
				had_error = true;
				add_token(TokenType::Illegal, "Unterminated character literal.");
			}
			else {
				had_error = true;
				add_token(TokenType::Illegal, "Character literal must contain only one character.");
				// Consume until the next quote to help parser recover
				while (!is_at_end() && peek() != '\'') {
					advance();
				}
				if (!is_at_end()) {
					advance(); // Consume the found closing quote
				}
			}

			return; // Handled errors, return
		}

		advance(); // Consume closing '
		add_token(TokenType::CharacterLiteral, std::string(1, ch));
	}

	void Lexer::read_string() {
		std::string buf; // Can also do from start to index - start

		while (peek() != '"' && !is_at_end()) {
			char ch = advance();

			if (ch == '\\') { // Start of escape sequence
				if (is_at_end()) {
					had_error = true;
					add_token(TokenType::Illegal, "Unterminated string literal.");
					return;
				}
				char escaped = advance();
				switch (escaped) {
				case 'n': buf += '\n'; break;
				case 't': buf += '\t'; break;
				case 'r': buf += '\r'; break;
				case '\\': buf += '\\'; break;
				case '\'': buf += '\''; break;
				case '"': buf += '"'; break;
				default:
					had_error = true;
					add_token(TokenType::Illegal, "Invalid escape sequence: \\" + std::string(1, escaped));
					while (peek() != '"' && !is_at_end()) {
						advance();
					}
					if (!is_at_end()) {
						advance(); // Consume closing "
					}
					return;
				}
			}
			else {
				buf += ch;
			}
		}

		if (is_at_end()) {
			had_error = true;
			add_token(TokenType::Illegal, "Unterminated string literal.");
			return;
		}

		advance(); // Consume closing "
		add_token(TokenType::StringLiteral, buf);
	}
	
	void Lexer::read_for_loop_pattern() {

	}

	void Lexer::read_number() {
		bool is_float = false;

		// Handle integer numbers first

		while (is_digit(peek())) {
			advance();
		}

		if ((peek() == 'i' || peek() == 'u' || peek() == 'f') && is_digit(peek_next())) {
			size_t suffix_start = index;
			advance(); // Consume the suffix char

			while (is_digit(peek())) {
				advance();
			}

			std::string suffix = input.substr(suffix_start, index - suffix_start);

			bool valid_int_suffix = (valid_int_suffixes.find(suffix) != valid_int_suffixes.end());
			bool valid_float_suffix = (valid_float_suffixes.find(suffix) != valid_float_suffixes.end());

			if (valid_float_suffix) {
				is_float = true; // Upgrade to float (e.g., 3f32)
			}
			else if (!valid_int_suffix) { // Not valid float or int suffix
				std::string malformed_literal = input.substr(start, index - start);
				had_error = true;
				add_token(TokenType::Illegal, "Invalid integer suffix: " + malformed_literal);
				return;
			}
		}

		// Check for range operator (integers only)

		if (peek() == '.' && peek_next() == '.') {
			if (is_float) { // e.g., 3f32.. is illegal
				advance();
				advance();
				std::string error_text = input.substr(start, index - start);
				had_error = true;
				add_token(TokenType::Illegal, "Range operator cannot follow a float literal: " + error_text);
				return;
			}
			else {
				add_token(TokenType::IntLiteral, input.substr(start, index - start));
				return; // Main loop consumes ..
			}	
		}

		// Handle float numbers

		if (peek() == '.' && !is_alpha(peek_next())) {
			is_float = true;
			advance(); // Consume the '.'

			while (is_digit(peek())) {
				advance();
			}
		}

		if (is_float) {
			if ((peek() == 'i' || peek() == 'u' || peek() == 'f') && is_digit(peek_next())) {
				size_t suffix_start = index;
				advance(); // Consume the suffix char

				while (is_digit(peek())) {
					advance();
				}

				std::string suffix = input.substr(suffix_start, index - suffix_start);

				if (valid_float_suffixes.find(suffix) == valid_float_suffixes.end()) {
						std::string malformed_literal = input.substr(start, index - start);
						add_token(TokenType::Illegal, "Invalid suffix for float literal: " + malformed_literal);
						had_error = true;
						return;
				}
			}
		}
		
		// Check for malformed numbers
	
		if (peek() == '.') {
			if (peek_next() == '.' && is_float) {
				advance();
				advance();
				std::string error_text = input.substr(start, index - start);
				had_error = true;
				add_token(TokenType::Illegal, "Range operator cannot follow a float literal: " + error_text);
				return;
			}
			else {
				advance();
				std::string error_text = input.substr(start, index - start);
				add_token(TokenType::Illegal, "Malformed number literal: " + error_text);
				had_error = true;
				return;
			}
		}

		// Otherwise add as float / int literal
		std::string num_as_str = input.substr(start, index - start);

		if (is_float) {
			add_token(TokenType::FloatLiteral, num_as_str);
		}
		else {
			add_token(TokenType::IntLiteral, num_as_str);
		}
	}

	void Lexer::read_identifier() {
		while (is_alnum(peek())) {
			advance();
		}

		std::string text = input.substr(start, index - start);
		TokenType type;
		auto it = keywords.find(text);

		if (it != keywords.end()) {
			type = it->second;
		}
		else {
			type = TokenType::Identifier;
		}

		add_token(type);
	}

	bool Lexer::is_digit(char c) const {
		return c >= '0' && c <= '9';
	}

	bool Lexer::is_alpha(char c) const {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	bool Lexer::is_alnum(char c) const {
		return is_alpha(c) || is_digit(c) || c == '_';
	}

	bool Lexer::is_symbol(char c) const {
		return (c >= '!' && c <= '/') ||	// 33–47
			(c >= ':' && c <= '@') ||		// 58–64
			(c >= '[' && c <= '`') ||		// 91–96
			(c >= '{' && c <= '~');			// 123–126
	}

	bool Lexer::is_space(char c) const {
		return c == ' ';
	}

	void Lexer::error(const std::string& message) {
		std::cerr << "[" << line << ":" << col << "] Error: " << message << std::endl;
	}

}
