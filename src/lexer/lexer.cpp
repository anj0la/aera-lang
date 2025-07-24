#include <aera/lexer/lexer.hpp>
#include <iostream>
#include <filesystem>

namespace aera::lexer {

	Lexer::Lexer(DiagnosticReporter& reporter, const std::string& filename, const std::string& source) : reporter_(reporter), filename(filename),
	source(source) {
		
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

	bool Lexer::has_error() const {
		return had_error;
	}

	std::vector<std::string> Lexer::read_lines_from_source() {

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

			// Ignore spaces, new line and carriage return

			case ' ':
			case '\r':
			case '\t':
			case '\n':
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
					had_error = true;
					std::string bad_char(1, ch);
					add_token(TokenType::Illegal, bad_char); // Unexpected character, not valid in language
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

	void Lexer::add_token(TokenType type, const std::string& lexeme, const SourceLocation loc, const std::optional<Value>& p_value) {
		tokens.push_back(Token(type, lexeme, start_location()));
	}

	Token(TokenType p_type, const std::string& p_lexeme, const SourceLocation& p_loc, const std::optional<Value>& p_value = std::nullopt,
		const std::optional<std::string>& p_suffix = std::nullopt)
		: type(p_type), lexeme(p_lexeme), loc(p_loc), value(p_value), suffix(p_suffix) {
	}

	void Lexer::add_token(TokenType type, const std::string& lexeme, Value literal) {
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

			std::string suffix = source.substr(suffix_start, index - suffix_start);

			bool valid_int_suffix = (valid_int_suffixes.find(suffix) != valid_int_suffixes.end());
			bool valid_float_suffix = (valid_float_suffixes.find(suffix) != valid_float_suffixes.end());

			if (valid_float_suffix) {
				is_float = true; // Upgrade to float (e.g., 3f32)
			}
			else if (!valid_int_suffix) { // Not valid float or int suffix
				std::string malformed_literal = source.substr(start, index - start);
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
				std::string error_text = source.substr(start, index - start);
				had_error = true;
				add_token(TokenType::Illegal, "Range operator cannot follow a float literal: " + error_text);
				return;
			}
			else {
				add_token(TokenType::IntLiteral, source.substr(start, index - start));
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

				std::string suffix = source.substr(suffix_start, index - suffix_start);

				if (valid_float_suffixes.find(suffix) == valid_float_suffixes.end()) {
						std::string malformed_literal = source.substr(start, index - start);
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
				std::string error_text = source.substr(start, index - start);
				had_error = true;
				add_token(TokenType::Illegal, "Range operator cannot follow a float literal: " + error_text);
				return;
			}
			else {
				advance();
				std::string error_text = source.substr(start, index - start);
				add_token(TokenType::Illegal, "Malformed number literal: " + error_text);
				had_error = true;
				return;
			}
		}

		// Otherwise add as float / int literal
		std::string num_as_str = source.substr(start, index - start);

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
