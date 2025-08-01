#include <aera/lexer/lexer.hpp>
#include <iostream>
#include <fstream>

namespace aera::lexer {

	Lexer::Lexer(DiagnosticReporter& reporter, const std::string& filename, const std::string& source) : reporter_(reporter), filename(filename),
	source(source) {
		read_lines_from_source(); // if the filepath doesn't exist, the vector is empty
	}

	std::vector<Token> Lexer::tokenize() {

		while (!is_at_end()) {
			start = index;
			start_col = col;
			read_token();
		}

		tokens.push_back(Token(TokenType::Eof, "", current_location()));
		return tokens;
	}

	void Lexer::read_lines_from_source() {

		std::ifstream file(filename);

		if (!file.is_open()) {
			return; // vector remains empty
		}

		std::string line;
		while (std::getline(file, line)) {
			source_lines.emplace_back(line);
		}

		file.close();
	}

	SourceLocation Lexer::current_location() const {
		return SourceLocation{ filename, line, col };
	}

	SourceLocation Lexer::start_location() const {
		return SourceLocation{ filename, line, start_col };
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

			// Add new line 
			case '\n': add_token(TokenType::Newline); break;

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
					std::string bad_char(1, ch);
					std::string msg = "unexpected character '" + bad_char + "'";
					reporter_.add_error(filename, 1, start_location(), msg, source_lines[line], "this character is not supported in the language");
					add_token(TokenType::Illegal, bad_char); // Add token so parser can handle error
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

	void Lexer::add_token(TokenType type, const std::string& lexeme) {
		tokens.push_back(Token(type, lexeme, start_location()));
	}

	void Lexer::add_token(TokenType type) {
		std::string text = source.substr(start, index - start);
		tokens.push_back(Token(type, text, start_location()));
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
			case '(': add_token(TokenType::LeftParen); break;
			case ')': add_token(TokenType::RightParen); break;
			case '{': add_token(TokenType::LeftBrace); break;
			case '}': add_token(TokenType::RightBrace); break;
			case '[': add_token(TokenType::LeftBracket); break;
			case ']': add_token(TokenType::RightBracket); break;
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
				std::string msg = "unterminated block comment";
				reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line], "block comments are enclosed with #>");
				add_token(TokenType::Illegal, "Unterminated block comment.");
				break;
			}
			if (peek() == '\n') {
				std::string msg = "unterminated block comment";
				reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line], "block comments are enclosed with #>");
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
			std::string msg = "empty character literal ''";
			reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line], "character literal must be a single character. did you mean to use a string literal?");
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
					std::string bad_escape = std::string(1, escaped);
					std::string msg = "invalid escape sequence \\" + bad_escape;
					reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line], "supported escape sequences are \\n, \\r, \\t, \\, \', and \"");
					add_token(TokenType::Illegal, "Invalid escape sequence: \\" + std::string(1, escaped));
					break;
			}
		}
		else if (!(is_alpha(ch) || is_digit(ch) || is_symbol(ch) || is_space(ch))) {
			if (ch == '\\') {
				std::string msg = "unterminated escape sequence in character literal";
				reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line]);
				add_token(TokenType::Illegal, "Unterminated escape sequence in character literal.");
			}
			else {
				std::string bad_char = std::string(1, ch);
				std::string msg = "invalid character in literal:" + bad_char;
				reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line]);
				add_token(TokenType::Illegal, "Invalid character in literal: " + bad_char);
			}
		}

		if (peek() != '\'') {
			if (is_at_end()) {
				std::string msg = "unterminated character literal.";
				reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line]);
				add_token(TokenType::Illegal, "Unterminated character literal.");
			}
			else {
				std::string msg = "character literal must contain only one character.";
				reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line]);
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
		add_token(TokenType::CharacterLiteral, std::string(1, ch), ch); // the lexeme is a string, the actual value is a char
	}

	void Lexer::read_string() {
		std::string buf; // Can also do from start to index - start

		while (peek() != '"' && !is_at_end()) {
			char ch = advance();

			if (ch == '\\') { // Start of escape sequence
				if (is_at_end()) {
					std::string msg = "unterminated string literal.";
					reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line]);
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
						std::string bad_escape = std::string(1, escaped);
						std::string msg = "invalid escape sequence: \\" + bad_escape;
						reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line]);
						add_token(TokenType::Illegal, "Invalid escape sequence: \\" + bad_escape);
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
			std::string msg = "unterminated string literal.";
			reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line]);
			add_token(TokenType::Illegal, "Unterminated string literal.");
			return;
		}

		advance(); // Consume closing "
		add_token(TokenType::StringLiteral, buf, buf); // the lexeme and literal are the same values
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
		advance(); // comsume 'x'

		if (!is_hex_digit(peek())) {
			// error: "hexadecimal number must have at least one digit after 0x"
			return;
		}

		while (is_hex_digit(peek())) {
			advance();
		}

		if (peek() == '.') {
			// error: "hexadecimal numbers cannot have decimal points"
			return;
		}

		std::string num_lexeme = source.substr(start, (index - start));
		add_token(TokenType::IntLiteral, num_lexeme, static_cast<int64_t>(std::stoll(num_lexeme, nullptr, 0))); // value as int64 (long long)
	}

	void Lexer::read_binary_number() {
		advance(); // consume '0'
		advance(); // comsume 'b'

		if (!is_binary_digit(peek())) {
			// error: "binary number must have at least one digit after 0b"
			return;
		}

		while (is_binary_digit(peek())) {
			advance();
		}

		if (peek() == '.') {
			// error: "binary numbers cannot have decimal points"
			return;
		}

		std::string num_lexeme = source.substr(start, (index - start));
		add_token(TokenType::IntLiteral, num_lexeme, static_cast<int64_t>(std::stoll(num_lexeme, nullptr, 0))); // value as int64 (long long)
	}

	void Lexer::read_octal_number() {
		advance(); // consume '0'
		advance(); // comsume 'o'

		if (!is_octal_digit(peek())) {
			// error: "octal number must have at least one digit after 0o"
			return;
		}

		while (is_octal_digit(peek())) {
			advance();
		}

		if (peek() == '.') {
			// error: "octal numbers cannot have decimal points"
			return;
		}

		std::string num_lexeme = source.substr(start, (index - start));
		add_token(TokenType::IntLiteral, num_lexeme, static_cast<int64_t>(std::stoll(num_lexeme, nullptr, 0))); // value as int64 (long long)
	}

	void Lexer::read_decimal_number() {
		bool is_float = false;
		std::string suffix = "";

		// 1. Read integer part
		while (is_digit(peek())) {
			advance();
		}
		// 2. Read fractional part
		if (peek() == '.' && !is_alpha(peek_next())) {
			is_float = true;
			advance(); // Consume the '.'

			while (is_digit(peek())) {
				advance();
			}
		}
		// 3. Read scientifical notation
		if (peek() == 'e' || peek() == 'E') {
			is_float = true; // handles cases like 3e-12
			advance(); // consume e / E
			if (peek() == '+' || peek() == '-') {
				advance(); // consume + / -
			}
			if (!is_digit(peek())) {
				// error: malformed scientific notation
				return;
			}
			while (is_digit(peek())) {
				advance();
			}
		}
		// 4. Validate fractional part
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
				std::string error_text = source.substr(start, index - start);
				std::string msg = "range operator cannot follow a float literal: " + error_text;
				reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line]);
				add_token(TokenType::Illegal, "Range operator cannot follow a float literal: " + error_text);
				return false;
			}
			else {
				advance();
				std::string error_text = source.substr(start, index - start);
				std::string msg = "malformed float literal: " + error_text;
				reporter_.add_error(filename, get_token_length(), start_location(), msg, source_lines[line]);
				add_token(TokenType::Illegal, "Malformed float literal: " + error_text);
				return false;
			}
		}
		return true;
	}

	void Lexer::read_identifier() {
		while (is_alnum(peek())) {
			advance();
		}

		std::string text = source.substr(start, index - start);
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
}
