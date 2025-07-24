#include <aera/parser/parser.hpp>

namespace aera::parser {

	void Parser::declaration() {

	}

	void fn_declaration();
	void var_declaration();
	void const_declaration();
	void struct_declaration();
	void class_declaration();
	void trait_declaration();
	void with_declaration();

	// Expressions

	std::unique_ptr<Expr>  Parser::expression() {
		return assignment();
	}

	std::unique_ptr<Expr> Parser::assignment() {
		auto expr = conditional();

		if (match_assignment_op()) { 
			if (!expr->is_lvalue()) {
				// ParseError: Invalid assignment target -> probably should track position
				return nullptr;
			}
			Token op = prev();
			auto rhs = assignment();
			return std::make_unique<Assignment>(std::move(expr), op, std::move(rhs));
		}
		return expr;
	}

	std::unique_ptr<Expr> Parser::conditional() {
		auto expr = logical_or();

		if (match(TokenType::If)) { 
			auto condition = expression(); 
			consume(TokenType::Else, "Expected 'else' after conditional expression");
			auto else_expr = conditional();

			return std::make_unique<Conditional>(std::move(expr),
				std::move(condition),
				std::move(else_expr));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::logical_or() {
		auto expr = logical_and();

		while (match(TokenType::PipePipe)) {
			Token op = prev();
			auto rhs = logical_and();

			expr = std::make_unique<Binary>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::logical_and() {
		auto expr = bitwise_or();

		while (match(TokenType::AmpAmp)) {
			Token op = prev();
			auto rhs = bitwise_or();

			expr = std::make_unique<Binary>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}
	
	std::unique_ptr<Expr> Parser::bitwise_or() {
		auto expr = bitwise_xor();

		while (match(TokenType::Pipe)) {
			Token op = prev();
			auto rhs = bitwise_xor();

			expr = std::make_unique<Binary>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::bitwise_xor() {
		auto expr = bitwise_and();

		while (match(TokenType::Caret)) {
			Token op = prev();
			auto rhs = bitwise_and();

			expr = std::make_unique<Binary>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::bitwise_and() {
		auto expr = equality();

		while (match(TokenType::Amp)) {
			Token op = prev();
			auto rhs = equality();

			expr = std::make_unique<Binary>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::equality() {
		auto expr = comparison();

		while (match(TokenType::EqualEqual, TokenType::ExclaimEqual)) {
			Token op = prev();
			auto rhs = comparison();

			expr = std::make_unique<Binary>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::comparison() {
		auto expr = shift();

		while (match(TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual)) {
			Token op = prev();
			auto rhs = shift();

			expr = std::make_unique<Binary>(std::move(expr), op, std::move(rhs));
		}

		return expr;

	}

	std::unique_ptr<Expr> Parser::shift() {
		auto expr = term();

		while (match(TokenType::GreaterGreater, TokenType::LessLess)) {
			Token op = prev();
			auto rhs = term();

			expr = std::make_unique<Binary>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::term() {
		auto expr = factor();

		while (match(TokenType::Plus, TokenType::Minus)) {
			Token op = prev();
			auto rhs = factor();

			expr = std::make_unique<Binary>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::factor() {
		auto expr = unary();

		while (match(TokenType::Star, TokenType::Slash)) {
			Token op = prev();
			auto rhs = unary();

			expr = std::make_unique<Binary>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::unary() {
		
		if (match(TokenType::Exclaim, TokenType::Minus, TokenType::Tilde, TokenType::Amp)) {
			Token op = prev();
			auto rhs = unary();
			return std::make_unique<Unary>(op, std::move(rhs));
		}

		return postfix();
	}

	std::unique_ptr<Expr> Parser::postfix() {

		auto expr = primary();

		while (true) {
			// Array access
			if (match(TokenType::LeftBracket)) {
				auto index = expression();
				consume(TokenType::RightBracket, "Expected ']' after array index");
				expr = std::make_unique<ArrayAccess>(std::move(expr), std::move(index));
			}
			// Field Access
			else if (match(TokenType::Period)) {
				Token name = consume(TokenType::Identifier, "Expected property name after '.'");
				expr = std::make_unique<FieldAccess>(std::move(expr), name);
			}
			// Arguments
			else if (match(TokenType::LeftParen)) {
				std::vector<std::unique_ptr<Expr>> args;
				if (!check(TokenType::RightParen)) {
					args = argument_list();
				}
				consume(TokenType::RightParen, "Expected ')' after arguments");
				expr = std::make_unique<FnCall>(std::move(expr), std::move(args));

			}

			else if (match(TokenType::Question)) {
				// Need to figure out optional types, but for now, do nothing
			}
			else {
				break;
			}
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::primary() {

		if (match(TokenType::IntLiteral)) {
			return integer_literal();
		}
		if (match(TokenType::FloatLiteral)) {
			return float_literal();
		}
		if (match(TokenType::CharacterLiteral)) {
			return std::make_unique<Literal>(prev().lexeme[0]); // converts string to char literal
		}
		if (match(TokenType::StringLiteral)) {
			return std::make_unique<Literal>(prev().lexeme);
		}
		if (match(TokenType::True)) {
			return std::make_unique<Literal>(true);
		}
		if (match(TokenType::False)) {
			return std::make_unique<Literal>(false);
		}

		if (match(TokenType::Identifier)) {
			return identifier();
		}

		if (match(TokenType::LeftParen)) {
			auto expr = expression();
			consume(TokenType::RightParen, "Expected ')' after expression");
			return std::make_unique<Grouping>(std::move(expr));
		}

			return nullptr; // Throw parse error, or print error idk
		}

	std::vector<std::unique_ptr<Expr>> Parser::argument_list() {
		std::vector<std::unique_ptr<Expr>> args;
		args.push_back(expression());

		while (match(TokenType::Comma)) {
			args.push_back(expression());
		}

		return args;
	}


	// Statements

	void statement();
	void expression_statement();
	void return_statement();
	void if_statement();
	void while_statement();
	void for_statement();
	void iterator_for_statement();
	void range_for_statement();
	void loop_statement();
	void block();

	// Other parsing functions

	template <class... T>
	bool match(T... type) {
		static_assert((... && std::is_same_v<T, TokenType>),
			"All arguments must be TokenType"); 
		if ((... || check(type))) {
			advance();
			return true;
		}
		return false;
	}

	bool match_assignment_op() {
		return match(TokenType::Equal, TokenType::PlusEqual, TokenType::MinusEqual,
			TokenType::StarEqual, TokenType::SlashEqual, TokenType::PercentEqual,
			TokenType::LessLessEqual, TokenType::GreaterGreaterEqual, TokenType::AmpEqual, 
			TokenType::PipeEqual, TokenType::CaretEqual, TokenType::TildeEqual);
	}

	bool Parser::check(TokenType type) {
		if (is_at_end()) {
			return false;
		}

		return peek().type == type;
	}

	Token Parser::consume(TokenType type, const std::string& message) {
		if (check(type)) {
			return advance();
		}

		// Throw an error here
		return Token(TokenType::Illegal, message, 1, 1); // REPLACE WITH ROBUST ERROR HANDLING
	}

	Token Parser::advance() {
		if (!is_at_end()) {
			current++;
			return prev();
		}
	}

	bool Parser::is_at_end() {
		return peek().type == TokenType::Eof;
	}

	Token Parser::peek() {
		return tokens[current];
	}

	Token Parser::prev() {
		return tokens[current - 1];
	}

	void sync(); // TO DO LATER
}