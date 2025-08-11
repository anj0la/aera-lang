#include <aera/lexer/lexer.hpp>
#include <iostream>
#include <fstream>

namespace aera::lexer {

	std::vector<Token> Lexer::tokenize() {

		while (!is_at_end()) {
			start = index;
			start_col = col;
			read_token();
		}

		tokens.push_back(Token(TokenType::Eof, "", current_location(), ""));
		return tokens;
	}

	SourceLocation Lexer::current_location() const {
		return SourceLocation{ source_context_.filename(), line, col};
	}

	SourceLocation Lexer::start_location() const {
		return SourceLocation{ source_context_.filename(), line, start_col };
	}

	char Lexer::advance() {
		char ch = source[index];

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
			case '(': case ')': case '{': case '}':
			case ',': case ';': case ':': read_punctuation(ch); break;

			// Operators
			case '+': case '-': case '*': case '/':
			case '=': case '!': case '<': case '>':
			case '&': case '|': case '%': case '^':
			case '.': case '?': case '@': read_operator(ch); break;

			// Line comments
			case '#':
				read_line_comment();
				break;

			// Handle new line
			case '\n': 
				if (paren_depth == 0 && brace_depth == 0 && bracket_depth == 0) {
					if (can_end_statement(prev_token().type)) {
						add_token(TokenType::Semicolon);
					}
				}
				break;

			// Ignore spaces and carriage return
			case ' ':
			case '\r':
			case '\t':
				break;				

			// Character literal
			case '\'': read_character(); break;

			// String literal
			case '"': read_string(); break;

			// Number literals (int, float) and identifiers
			default: 
				if (is_digit(ch)) {
					read_number(); 
				}
				else if (is_alpha(ch)) {
					read_identifier();
				}
				else {
					std::string msg = "unexpected character '" + std::string(1, ch); +"'";
					error(msg, "this character is not supported in the language");
				}
				break;
		}
	}

	char Lexer::peek() {
		if (is_at_end()) {
			return '\0';
		}

		return source[index];
	}

	char Lexer::peek_next() {
		if (index + 1 >= source.length()) {
			return '\0';
		}

		return source[index + 1];
	}

	Token Lexer::prev_token() {
		return tokens[tokens.size() - 1];
	}

	bool Lexer::is_at_end() {
		return index >= source.length();
	}

	int Lexer::current_line() const {
		return line;
	}

	int Lexer::current_column() const {
		return col;
	}

	int Lexer::get_token_length() const {
		return source.substr(start, index - start).length();
	}

	void Lexer::add_token(TokenType type, const std::string& lexeme, const Value& literal) {
		tokens.push_back(Token(type, lexeme, start_location(), literal));
	}

	void Lexer::add_token(TokenType type) {
		std::string lexeme = source.substr(start, index - start);
		tokens.push_back(Token(type, lexeme, start_location(), lexeme));
	}

	bool Lexer::match(char expected) {
		if (is_at_end()) {
			return false;
		}

		if (source[index] != expected) {
			return false;
		}

		advance(); // to handle line and col
		return true;
	}

	void Lexer::read_punctuation(char c) {
		switch (c) {
			case '(': 
				paren_depth++;
				add_token(TokenType::LeftParen); 
				break;
			case ')': 
				paren_depth--;
				add_token(TokenType::RightParen); 
				break;
			case '{': 
				brace_depth++;
				add_token(TokenType::LeftBrace); 
				break;
			case '}': 
				brace_depth--;
				add_token(TokenType::RightBrace); 
				break;
			case '[': 
				bracket_depth++;
				add_token(TokenType::LeftBracket); 
				break;
			case ']': 
				bracket_depth--;
				add_token(TokenType::RightBracket); 
				break;
			case ',': add_token(TokenType::Comma); break;
			case ';': add_token(TokenType::Semicolon); break;
			case ':': add_token(TokenType::Colon); break;
		}
	}

	void Lexer::read_operator(char c) {
		switch (c) {
			// Longest tokens (length <= 3)

			case '.':
				if (match('.')) { // Handles ..= and .. tokens
					add_token((match('=') ? TokenType::PeriodPeriodEqual : TokenType::PeriodPeriod));
				}
				else { // Handles . token
					add_token(TokenType::Period);
				}
				break;

			case '<':
				if (match('<')) { // Handle <<= and << tokens
					add_token((match('=') ? TokenType::LessLessEqual : TokenType::LessLess));
				}
				else if (match('#')) { // Block comments start with <#
					read_block_comment();
				}
				else { // Handle < and <= tokens
					add_token((match('=') ? TokenType::LessEqual : TokenType::Less));
				}
				break;

			case '>':
				if (match('>')) { // Handle >>= and >> tokens
					add_token((match('>') ? TokenType::GreaterGreaterEqual : TokenType::GreaterGreater));
				}
				else { // Handle > and >= tokens
					add_token((match('=') ? TokenType::GreaterEqual : TokenType::Greater));
				}
				break;

			// Multi-tokens (len <= 2)

			case '+':
				if (match('+')) {
					add_token(TokenType::PlusPlus);
				}
				else {
					add_token((match('=') ? TokenType::PlusEqual : TokenType::Plus));
				}
				break;
				
			case '-':
				if (match('>')) {
					add_token(TokenType::MinusGreater);
				}
				else if (match('-')) {
					add_token(TokenType::MinusMinus);
				}
				else {
					add_token((match('=') ? TokenType::MinusEqual : TokenType::Minus));
				}
				break;

			case '*':
				add_token((match('=') ? TokenType::StarEqual : TokenType::Star));
				break;

			case '/':
				add_token((match('=') ? TokenType::SlashEqual : TokenType::Slash));
				break;

			case '%':
				add_token((match('=') ? TokenType::PercentEqual : TokenType::Percent));
				break;

			case '!':
				add_token((match('=') ? TokenType::ExclaimEqual : TokenType::Exclaim));
				break;

			case '=':
				add_token((match('=') ? TokenType::EqualEqual : TokenType::Equal));
				break;
			
			case '&':
				if (match('&')) { // Handles logical and (&&) token
					add_token(TokenType::AmpAmp);  
				}
				else { // Handles bitwise and tokens
					add_token((match('=') ? TokenType::AmpEqual : TokenType::Amp));
				}
				break;

			case '|': 
				if (match('|')) { // Handles logical or (||) token
					add_token(TokenType::PipePipe);
				}
				else { // Handles bitwise or tokens
					add_token((match('=') ? TokenType::PipeEqual : TokenType::Pipe));
				}
				break;

			case '^':
				add_token((match('=') ? TokenType::CaretEqual : TokenType::Caret));
				break;

			case '~':
				add_token((match('=') ? TokenType::TildeEqual : TokenType::Tilde));
				break;
			
			// Simple operators (len == 1)

			case '?': add_token(TokenType::Question); break;
			case '@': add_token(TokenType::At); break;
		}

	}

	void Lexer::read_line_comment() {
		while (peek() != '\n' && !is_at_end()) {
			advance();
		}
	}

	void Lexer::read_block_comment() {
		while (true) {
			if (is_at_end()) {
				error("unterminated block comment", "block comments are enclosed with #>");
				break;
			}
			if (peek() == '\n') {
				error("unterminated block comment", "block comments are enclosed with #>");
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
			advance(); // consume '
			error("empty character literal ''", "character literal must be a single character. did you mean to use a string literal?");
			return;
		}

		char ch = advance(); // consume char

		if (ch == '\\' && !is_at_end()) { // handle escape sequences
			char escaped = advance();
			switch (escaped) {
				case 'n': ch = '\n'; break;
				case 't': ch = '\t'; break;
				case 'r': ch = '\r'; break;
				case '\\': ch = '\\'; break;
				case '\'': ch = '\''; break;
				case '"': ch = '"'; break;
				default:
					std::string msg = "invalid escape sequence \\" + std::string(1, escaped);
					error(msg);
					break;
			}
		}
		else if (!(is_printable(ch))) {
			if (ch == '\\') {
				error("unterminated escape sequence in character literal");
			}
			else {
				std::string msg = "invalid character in literal:" + std::string(1, ch);
				error(msg);
			}
		}

		if (peek() != '\'') {
			if (is_at_end()) {
				error("unterminated character literal");
			}
			else { // Consume until the next quote to help parser recover
				error("character literal must contain only one character");
				while (!is_at_end() && peek() != '\'') {
					advance();
				}
				if (!is_at_end()) {
					advance(); // consume the found closing quote
				}
			}
			return; // handled errors, return
		}

		advance(); // consume closing '
		add_token(TokenType::CharacterLiteral, std::string(1, ch), ch);
	}

	void Lexer::read_string() {
		std::string buf;

		while (peek() != '"' && !is_at_end()) {
			char ch = advance();

			if (ch == '\\') { // start of escape sequence
				if (is_at_end()) {
					error("unterminated string literal");
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
						std::string msg = "invalid escape sequence: \\" + std::string(1, escaped);
						error(msg);
						// Consume until the next quote to help parser recover
						while (peek() != '"' && !is_at_end()) {
							advance();
						}
						if (!is_at_end()) {
							advance(); // consume closing "
						}
						return;
				}
			}
			else {
				buf += ch;
			}
		}
		if (is_at_end()) {
			error("unterminated string literal");
			return;
		}

		advance(); // consume closing "
		add_token(TokenType::StringLiteral, buf, buf);
	}

	void Lexer::read_number() {
		if (peek() == '0') {
			if (peek_next() == 'x' || peek_next() == 'X') {
				read_hexademical_number();
			}
			else if (peek_next() == 'b' || peek_next() == 'B') {
				read_binary_number();
			}
			else if (peek_next() == 'o' || peek_next() == 'O') {
				read_octal_number();
			}
			else {
				read_decimal_number();
			}
		}
		else {
			read_decimal_number();
		}
	}

	void Lexer::read_hexademical_number() {
 		advance(); // consume '0'
		advance(); // consume 'x'

		if (!is_hex_digit(peek())) {
			error("hexadecimal number must have at least one digit after 0x");
			return;
		}

		while (is_hex_digit(peek())) {
			advance();
		}

		if (peek() == '.') {
			error("hexadecimal numbers cannot have decimal points");
			return;
		}

		std::string num_lexeme = source.substr(start, (index - start));
		add_token(TokenType::IntLiteral, num_lexeme, static_cast<int64_t>(std::stoll(num_lexeme, nullptr, 0))); // value as int64 (long long)
	}

	void Lexer::read_binary_number() {
		advance(); // consume '0'
		advance(); // consume 'b'

		if (!is_binary_digit(peek())) {
			error("binary number must have at least one digit after 0b");
			return;
		}

		while (is_binary_digit(peek())) {
			advance();
		}

		if (peek() == '.') {
			error("binary numbers cannot have decimal points");
			return;
		}

		std::string num_lexeme = source.substr(start, (index - start));
		add_token(TokenType::IntLiteral, num_lexeme, static_cast<int64_t>(std::stoll(num_lexeme, nullptr, 0))); // value as int64 (long long)
	}

	void Lexer::read_octal_number() {
		advance(); // consume '0'
		advance(); // consume 'o'

		if (!is_octal_digit(peek())) {
			error("octal number must have at least one digit after 0o");
			return;
		}

		while (is_octal_digit(peek())) {
			advance();
		}

		if (peek() == '.') {
			error("octal numbers cannot have decimal points");
			return;
		}

		std::string num_lexeme = source.substr(start, (index - start));
		add_token(TokenType::IntLiteral, num_lexeme, static_cast<int64_t>(std::stoll(num_lexeme, nullptr, 0))); // value as int64 (long long)
	}

	void Lexer::read_decimal_number() {
		bool is_float = false;

		while (is_digit(peek())) { // integer part
			advance();
		}
		
		if (peek() == '.' && !is_alpha(peek_next())) { // fractional part
			is_float = true;
			advance(); // consume '.'

			while (is_digit(peek())) {
				advance();
			}
		}

		if (peek() == 'e' || peek() == 'E') { // scientific notation
			is_float = true;
			advance(); // consume e / E
			if (peek() == '+' || peek() == '-') {
				advance(); // consume + / -
			}
			if (!is_digit(peek())) {
				error("malformed scientific notation");
				return;
			}
			while (is_digit(peek())) {
				advance();
			}
		}

		if (is_valid_fractional_part()) { // an empty fractional part is still valid
			std::string num_lexeme = source.substr(start, (index - start));
			if (is_float) {
				add_token(TokenType::FloatLiteral, num_lexeme, std::stod(num_lexeme)); // value as float64 (double)
			}
			else {
				add_token(TokenType::IntLiteral, num_lexeme, static_cast<int64_t>(std::stoll(num_lexeme))); // value as int64 (long long)
			}
		}
	}

	bool Lexer::is_valid_fractional_part() {
		if (peek() == '.') {
			if (peek_next() == '.') { 
				advance();
				advance();
				std::string msg = "range operator cannot follow a float literal: " + source.substr(start, index - start);
				error(msg);
				return false;
			}
			else {
				advance();
				std::string msg = "malformed float literal: " + source.substr(start, index - start);
				error(msg);
				return false;
			}
		}
		return true;
	}

	void Lexer::read_identifier() {
		while (is_alnum(peek())) {
			advance();
		}

		std::string lexeme = source.substr(start, index - start);
		TokenType type;
		auto it = keywords.find(lexeme);

		if (it != keywords.end()) {
			type = it->second;
		}
		else {
			type = TokenType::Identifier;
		}

		add_token(type);
	}

	bool can_end_statement(TokenType type) {
		switch (type) {
			case TokenType::Identifier:
			case TokenType::IntLiteral:
			case TokenType::FloatLiteral:
			case TokenType::CharacterLiteral:
			case TokenType::StringLiteral:
			case TokenType::True:
			case TokenType::False:
			case TokenType::Break:
			case TokenType::Continue:
			case TokenType::Return:
			case TokenType::RightParen:
			case TokenType::RightBrace:
			case TokenType::RightBracket:
				return true;
			default:
				return false;
		}
	}

	bool Lexer::is_digit(char c) const {
		return c >= '0' && c <= '9';
	}

	bool Lexer::is_hex_digit(char c) const {
		return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
	}

	bool Lexer::is_binary_digit(char c) const {
		return c == '0' || c == '1';
	}

	bool Lexer::is_octal_digit(char c) const {
		return c >= '0' && c <= '7';
	}

	bool Lexer::is_alpha(char c) const {
		return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
	}

	bool Lexer::is_alnum(char c) const {
		return is_alpha(c) || is_digit(c) || c == '_';
	}

	bool Lexer::is_symbol(char c) const {
		return (c >= '!' && c <= '/') ||	
			(c >= ':' && c <= '@') ||		
			(c >= '[' && c <= '`') ||		
			(c >= '{' && c <= '~');			
	}

	bool Lexer::is_space(char c) const {
		return c == ' ';
	}

	bool Lexer::is_printable(char c) const {
		return is_alpha(c) || is_digit(c) || is_symbol(c) || is_space(c);
	}

	void Lexer::error(const std::string& msg, const std::string& note) {
		reporter_.add_error(source_context_.filename(), get_token_length(), start_location(), msg, source_context_.get_line(line), note);
		add_token(TokenType::Illegal);
	}
}
