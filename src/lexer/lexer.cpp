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
			case '.': add_token(TokenType::Period); break;
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
					error("Unexpected character.");
					add_token(TokenType::Illegal);
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
		while (!is_at_end()) {
			if (peek() == '\n') {
				error("Unterminated block comment.");
				break;
			}
			if (peek() == '#' && peek_next() != '>') {
				error("Missing closing > in block comment.");
				advance(); // Consume '#'
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
			error("Empty character literal");
			advance();
			return;
		}

		if (is_at_end()) {
			error("Unterminated character literal");
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
				error("Invalid escape sequence : \\" + std::string(1, escaped));
				break;
			}
		}
		else if (!(is_alpha(ch) || is_digit(ch) || is_symbol(ch) || is_space(ch))) {
			if (ch == '\\') {
				error("Unterminated escape sequence in character literal");
			}
			else {
				error("Invalid character in literal: " + std::string(1, ch));
			}
		}

		if (peek() != '\'') {
			if (is_at_end()) {
				error("Unterminated character literal");
			}
			else {
				error("Character literal is too long; must contain only one character");
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
		bool had_error = false;
		std::string buf; 

		while (true) {

			if (is_at_end()) {
				error("Unterminated string."); 
				return; 
			}

			char ch = advance();

			if (ch == '"') {
				break; // Found closing double quote
			}
			else if (ch == '\\') { // Start of escape sequence
				if (is_at_end()) {
					error("Unterminated string after escape character.");
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
					error("Invalid escape sequence : \\" + std::string(1, escaped));
					break;
				}
			}
			else {
				buf += ch;
			}
		}

		if (!had_error) {
			add_token(TokenType::StringLiteral, buf);
		}
	}
	
	void Lexer::read_number() {
		bool is_float = false;

		while (is_digit(peek())) {
			advance();
		}

		if ((peek() == 'u' && is_digit(peek_next())) || (peek() == 'i' && is_digit(peek_next()))) { // Case sensitive
			advance(); // consume the suffix char

			while (is_digit(peek())) {
				advance();
			}
		}
		else if (peek() == '.' && is_digit(peek_next())) {
			is_float = true;
			advance();

			while (is_digit(peek())) {
				advance();
			}

			// Check for float suffix

			if (peek() == 'f' && is_digit(peek_next())) { // Case sensitive
				advance(); // consume the suffix char

				while (is_digit(peek())) {
					advance();
				}
			}
		}

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
		had_error = true;
	}

}
