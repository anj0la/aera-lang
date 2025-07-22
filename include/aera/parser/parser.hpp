#pragma once

#include <aera/parser/ast.hpp>
#include <aera/lexer/lexer.hpp>
#include <stdexcept> // For now, Parse Errors throw an std::runtime exception
#include <utility>  
#include <type_traits>

namespace aera::parser {

	class Parser {
	public:
		Parser(const std::vector<Token>& p_tokens) : tokens(p_tokens) {}
		~Parser() = default;

		struct ParseError : public std::runtime_error {
			using std::runtime_error::runtime_error;
		};

	private:
		const std::vector<Token> tokens;
		int current = 1;

		// Declarations

		void declaration();
		void fn_declaration();
		void var_declaration();
		void const_declaration();
		void struct_declaration();
		void class_declaration();
		void trait_declaration();
		void with_declaration();

		// Expressions

		std::unique_ptr<Expr> expression();
		std::unique_ptr<Expr> assignment();
		std::unique_ptr<Expr> conditional();
		std::unique_ptr<Expr> logical_or();
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
		bool match(T... type);
		Token consume();
		Token advance();
		bool is_at_end();
		Token peek();
		Token prev();
		void sync();
		ParseError error(Token token, std::string message);
	};

}