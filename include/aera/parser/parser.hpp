#pragma once

#include <aera/ast.hpp>
#include <aera/lexer/lexer.hpp>
#include <aera/diagnostics.hpp>
#include <aera/source_context.hpp>
#include <optional>
#include <utility>  
#include <type_traits>

namespace aera::parser {

	class Parser {
	public:
		Parser(const SourceContext& source_context, DiagnosticReporter& reporter, const std::vector<Token>& p_tokens) : 
			source_context_(source_context), reporter_(reporter), tokens(p_tokens) {}
		~Parser() = default;

		std::vector<std::unique_ptr<Decl>> parse();

	private:
		const SourceContext& source_context_;
		DiagnosticReporter& reporter_;
		const std::vector<Token> tokens;
		int current = 1;

		// Declarations

		std::unique_ptr<Decl> declaration();
		std::unique_ptr<Decl> fn_declaration();
		std::unique_ptr<Decl> var_declaration();
		std::unique_ptr<Decl> const_declaration();
		std::unique_ptr<Decl> struct_declaration();
		std::unique_ptr<Decl> field_declaration();
		std::unique_ptr<Decl> class_declaration();
		std::vector<ClassMember> class_body();
		std::unique_ptr<Decl> trait_declaration();
		std::unique_ptr<Decl> with_declaration();

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
		std::unique_ptr<Expr> cast();
		std::unique_ptr<Expr> postfix();
		std::unique_ptr<Expr> primary();
		std::vector<std::unique_ptr<Expr>> argument_list();

		// Statements

		std::unique_ptr<Stmt> statement();
		std::unique_ptr<Stmt> expression_statement();
		std::unique_ptr<Stmt> return_statement();
		std::unique_ptr<Stmt> if_statement();
		std::unique_ptr<Stmt> while_statement();
		std::unique_ptr<Stmt> for_statement();
		std::unique_ptr<Stmt> loop_statement();
		std::unique_ptr<Stmt> match_statement();
		std::optional<MatchClause> match_clause();
		std::unique_ptr<BlockStmt> block();

		// Types

		std::unique_ptr<Type> parse_type();
		std::unique_ptr<Type> parse_base_type();
		std::unique_ptr<Type> parse_primitive_type(const std::string& name);
		std::unique_ptr<Type> parse_builtin_type(const std::string& name);
		std::unique_ptr<Type> parse_generic_type(const std::string& name);
		std::unique_ptr<Type> parse_user_type(const std::string& name);
		std::vector<int> parse_array_dimensions();

		bool is_primitive_type(const std::string& name) const;

		// Other parsing functions

		template <class... T>
		bool match(T... type);
		bool check(TokenType type); 
		bool check_next(TokenType type);
		std::optional<Token> consume(TokenType type);
		Token advance();
		bool is_at_end();
		Token peek();
		Token peek_next();
		Token prev();
		void sync();
		bool check_field_declaration();
		bool check_fn_declaration();
		void error(const std::string& msg, const std::string& note = "");
		void skip_newlines();
	};

}