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

	std::unique_ptr<Expr> assignment() {
		auto expr = conditional();

		if (match_assignment_op()) { 
			if (!expr->is_lvalue()) {
				// ParseError: Invalid assignment target -> probably should track position
				return nullptr;
			}
			Token op = prev();  // assignment operator token
			auto rhs = assignment();  // right-associative
			return std::make_unique<Assignment>(std::move(expr), op, std::move(rhs));
		}
		return expr;
	}

	std::unique_ptr<Expr> conditional() {

	}

	void logical_or();
	void logical_and();
	void bitwise_or();
	void bitwise_xor();
	void bitwise_and();
	void equality();
	void comparision();
	void shift();
	void term();
	void factor();
	void unary();
	void postfix();
	void primary();
	void argument_list();

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
			"All arguments must be TokenType."); 
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

	Token consume();
	Token advance();
	bool is_at_end();
	Token peek();
	Token prev();
	void sync();
}