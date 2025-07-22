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
		std::unique_ptr<Expr> logical_and();
		std::unique_ptr<Expr> bitwise_or();
		std::unique_ptr<Expr> bitwise_xor();
		std::unique_ptr<Expr> bitwise_and();
		std::unique_ptr<Expr> equality();
		std::unique_ptr<Expr> comparison();
		std::unique_ptr<Expr> shift();
		std::unique_ptr<Expr> term();
		std::unique_ptr<Expr> factor();
		std::unique_ptr<Expr> unary();
		std::unique_ptr<Expr> postfix();
		std::unique_ptr<Expr> primary();
		std::vector<std::unique_ptr<Expr>> argument_list();

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
		bool check(TokenType type); 
		Token consume(TokenType type, const std::string& message);
		Token advance();
		bool is_at_end();
		Token peek();
		Token prev();
		void sync();
		ParseError error(Token token, std::string message);
	};

}