#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cstdint>
#include <variant>
#include <optional>
#include <vector>

#include <aera/token.hpp>
#include <aera/types.hpp>

namespace aera {

	// Forward Declarations

	struct Program;
	struct Decl;
	struct FnDecl;
	struct VarDecl;
	struct ConstDecl;
	struct StructDecl;
	struct FieldDecl;
	struct ClassDecl;
	struct ClassMember;
	struct TraitDecl;
	struct WithDecl;
	struct Expr;
	struct Assignment;
	struct Conditional;
	struct Binary;
	struct Unary;
	struct Cast;
	struct ArrayAcces;
	struct FnCall;
	struct FieldAccess;
	struct Grouping;
	struct Literal;
	struct Identifier;
	struct Stmt;
	struct ExprStmt;
	struct ReturnStmt;
	struct IfStmt;
	struct WhileStmt;
	struct ForStmt;
	struct IteratorForStmt;
	struct RangeForStmt;
	struct LoopStmt;
	struct MatchStmt;
	struct MatchClause;
	struct BlockStmt;

	// Visitor (Declarations)

	struct DeclVisitor {
		virtual ~DeclVisitor() = default;

		virtual void visit_fn_decl(const FnDecl& decl) = 0;
		virtual void visit_var_decl(const VarDecl& decl) = 0;
		virtual void visit_const_decl(const ConstDecl& decl) = 0;
		virtual void visit_struct_decl(const StructDecl& decl) = 0;
		virtual void visit_class_decl(const ClassDecl& decl) = 0;
		virtual void visit_field_decl(const FieldDecl& decl) = 0;
		virtual void visit_trait_decl(const TraitDecl& decl) = 0;
		virtual void visit_with_decl(const WithDecl& decl) = 0;
	};

	// Declarations

	struct Program {
		std::vector<std::unique_ptr<Decl>> decls;
	};

	struct Decl : Stmt { // Abstract
		virtual void accept(DeclVisitor& visitor) const = 0;
		virtual ~Decl() = default;
	};

	struct FnDecl : Decl {
		Token name;
		std::vector<Token> decorators;
		std::optional<TokenType> visibility_modifier;
		std::optional<TokenType> behaviour_modifier; // currently supporting modifies, could have others as well (turn into vector)
		std::vector<std::pair<Token, std::unique_ptr<Type>>> params;
		std::optional<std::unique_ptr<Type>> return_type;
		std::vector<std::unique_ptr<Stmt>> body;

		void accept(DeclVisitor& visitor) const override {
			visitor.visit_fn_decl(*this);
		}
	};

	struct VarDecl : Decl { // Immutable by default, CAN be changed
		Token name;
		bool is_mutable = false;
		std::optional<std::unique_ptr<Type>> decl_type; // optionally declared by user
		std::optional<std::unique_ptr<Expr>> initializer;

		void accept(DeclVisitor& visitor) const override {
			visitor.visit_var_decl(*this);
		}
	};

	struct ConstDecl : Decl { // IMMUTABLE, cannot be changed
		Token name;
		std::unique_ptr<Type> type;
		std::unique_ptr<Expr> initializer;

		void accept(DeclVisitor& visitor) const override {
			visitor.visit_const_decl(*this);
		}
	}; 

	struct StructDecl : Decl {
		Token name;
		std::vector<std::unique_ptr<FieldDecl>> fdecls;

		void accept(DeclVisitor& visitor) const override {
			visitor.visit_struct_decl(*this);
		}
	};

	struct FieldDecl : Decl {
		Token name;
		std::unique_ptr<Type> type;
		std::optional<std::unique_ptr<Expr>> initializer;

		void accept(DeclVisitor& visitor) const override {
			visitor.visit_field_decl(*this);
		}
	};

	struct ClassDecl : Decl {
		Token name;
		std::optional<Token> parent_class;
		std::vector<ClassMember> members;

		void accept(DeclVisitor& visitor) const override {
			visitor.visit_class_decl(*this);
		}
	};

	struct ClassMember {
		enum class ClassTypeKind { Field, Function };
		ClassTypeKind type;
		std::unique_ptr<Decl> decl;

		static ClassMember make_field(std::unique_ptr<FieldDecl> field) {
			{ ClassTypeKind::Field, std::move(field) };
		}
		static ClassMember make_function(std::unique_ptr<FnDecl> fn) {
			{ ClassTypeKind::Function, std::move(fn) };
		}
	};

	struct TraitDecl : Decl {
		Token name;
		std::vector<std::unique_ptr<FnDecl>> fndecls;

		void accept(DeclVisitor& visitor) const override {
			visitor.visit_trait_decl(*this);
		}
	};

	struct WithDecl : Decl {
		Token trait_name; // the trait
		Token type_name;  // the user-defined type to apply the trait to
		std::vector<std::unique_ptr<FnDecl>> fndecls;

		void accept(DeclVisitor& visitor) const override {
			visitor.visit_with_decl(*this);
		}
	};

	// Visitor (Expressions)

	struct ExprVisitor {
		virtual ~ExprVisitor() = default;

		virtual void visit_assignment_expr(const Assignment& expr) = 0;
		virtual void visit_conditional_expr(const Conditional& expr) = 0;
		virtual void visit_binary_expr(const Binary& expr) = 0;
		virtual void visit_unary_expr(const Unary& expr) = 0;
		virtual void visit_cast_expr(const Cast& expr) = 0;
		virtual void visit_array_access_expr(const ArrayAccess& expr) = 0;
		virtual void visit_fn_call_expr(const FnCall& expr) = 0;
		virtual void visit_field_access_expr(const FieldAccess& expr) = 0;
		virtual void visit_grouping_expr(const Grouping& expr) = 0;
		virtual void visit_literal_expr(const Literal& expr) = 0;
		virtual void visit_identifier_expr(const Identifier& expr) = 0;
	};

	// Expressions

	struct Expr {
		virtual bool is_lvalue() const { 
			return false; 
		}
		virtual ~Expr() = default;
		virtual void accept(ExprVisitor& visitor) const = 0;
	}; 

	struct Assignment : Expr {
		std::unique_ptr<Expr> expr;
		Token op;
		std::unique_ptr<Expr> rhs;

		void accept(ExprVisitor& visitor) const override {
			visitor.visit_assignment_expr(*this);
		}
	};

	struct Conditional : Expr {
		std::unique_ptr< Expr> conditional;
		std::unique_ptr< Expr> true_expr;
		std::unique_ptr< Expr> false_expr;

		void accept(ExprVisitor& visitor) const override {
			visitor.visit_conditional_expr(*this);
		}
	};

	struct Binary : Expr {
		std::unique_ptr< Expr> lhs;
		Token op;
		std::unique_ptr< Expr> rhs;

		void accept(ExprVisitor& visitor) const override {
			visitor.visit_binary_expr(*this);
		}
	};

	struct Unary : Expr {
		Token op;
		std::unique_ptr< Expr> rhs;

		void accept(ExprVisitor& visitor) const override {
			visitor.visit_unary_expr(*this);
		}
	};

	struct Cast : Expr {
		std::unique_ptr<Expr> expr;
		std::unique_ptr<Type> target_type;

		void accept(ExprVisitor& visitor) const override {
			visitor.visit_cast_expr(*this);
		}
	};

	struct ArrayAccess : Expr {
		std::unique_ptr< Expr> expr;
		int64_t index;

		void accept(ExprVisitor& visitor) const override {
			visitor.visit_array_access_expr(*this);
		}

		bool is_lvalue() const override { 
			true; 
		}
	};

	struct FnCall : Expr {
		std::unique_ptr< Expr> callee;
		Token paren;
		std::vector<std::unique_ptr< Expr>> args;

		void accept(ExprVisitor& visitor) const override {
			visitor.visit_fn_call_expr(*this);
		}
	};

	struct FieldAccess : Expr {
		std::unique_ptr< Expr> obj;
		Token name;

		void accept(ExprVisitor& visitor) const override {
			visitor.visit_field_access_expr(*this);
		}

		bool is_lvalue() const override { 
			true; 
		}

	};

	struct Grouping : Expr {
		std::unique_ptr< Expr> expr;

		void accept(ExprVisitor& visitor) const override {
			visitor.visit_grouping_expr(*this);
		}
	};

	struct Literal : Expr {
		Token token;
		std::unique_ptr<Type> type;
		
		void accept(ExprVisitor& visitor) const override {
			visitor.visit_literal_expr(*this);
		}
	};

	struct Identifier : Expr {
		Token name;
		
		void accept(ExprVisitor& visitor) const override {
			visitor.visit_identifier_expr(*this);
		}

		bool is_lvalue() const override { 
			true; 
		}
	};

	// Visitor (Statements)
	
	struct StmtVisitor {
		virtual ~StmtVisitor() = default;
		virtual void visit_expr_stmt(const ExprStmt& stmt) = 0;
		virtual void visit_return_stmt(const ReturnStmt& stmt) = 0;
		virtual void visit_if_stmt(const IfStmt& stmt) = 0;
		virtual void visit_while_stmt(const WhileStmt& stmt) = 0;
		virtual void visit_iterator_for_stmt(const IteratorForStmt& stmt) = 0;
		virtual void visit_range_for_stmt(const RangeForStmt& stmt) = 0;
		virtual void visit_loop_stmt(const LoopStmt& stmt) = 0;
		virtual void visit_match_stmt(const MatchStmt& stmt) = 0;
		virtual void visit_block_stmt(const BlockStmt& stmt) = 0;
	};

	// Statements

	struct Stmt { // Abstract
		virtual void accept(StmtVisitor& visitor) const = 0;
		virtual ~Stmt() = default;
	}; 

	struct ExprStmt : Stmt {
		std::unique_ptr<Expr> expr;

		void accept(StmtVisitor& visitor) const override {
			visitor.visit_expr_stmt(*this);
		}
	};

	struct ReturnStmt : Stmt {
		Token keyword;
		std::unique_ptr<Expr> value;

		void accept(StmtVisitor& visitor) const override {
			visitor.visit_return_stmt(*this);
		}
	};

	struct IfStmt : Stmt {
		std::unique_ptr<Expr> condition;
		std::unique_ptr<Stmt> then_branch;
		std::unique_ptr<Stmt> else_branch;

		void accept(StmtVisitor& visitor) const override {
			visitor.visit_if_stmt(*this);
		}
	};

	struct WhileStmt : Stmt {
		std::unique_ptr<Expr> condition;
		std::unique_ptr<Stmt> body;

		void accept(StmtVisitor& visitor) const override {
			visitor.visit_while_stmt(*this);
		}
	};

	struct ForStmt : Stmt {
		Token name;
		std::unique_ptr<Stmt> body;
	};

	struct IteratorForStmt : ForStmt {
		std::unique_ptr<Expr> collection;

		void accept(StmtVisitor& visitor) const override {
			visitor.visit_iterator_for_stmt(*this);
		}
	};

	struct RangeForStmt : ForStmt {
		std::unique_ptr<Expr> start_expr;
		std::unique_ptr<Expr> end_expr;

		void accept(StmtVisitor& visitor) const override {
			visitor.visit_range_for_stmt(*this);
		}
	};

	struct LoopStmt : Stmt {
		std::unique_ptr<Stmt> body;

		void accept(StmtVisitor& visitor) const override {
			visitor.visit_loop_stmt(*this);
		}
	};

	struct MatchStmt : Stmt {
		std::unique_ptr<Expr> expr;
		std::vector<MatchClause> clauses;

		void accept(StmtVisitor& visitor) const override {
			visitor.visit_match_stmt(*this);
		}
	};

	struct MatchClause {
		std::unique_ptr<Expr> pattern;  // Can be Literal or Identifier
		std::unique_ptr<Expr> expression;
	};

	struct BlockStmt : Stmt {
		std::vector<std::unique_ptr<Stmt>> stmts;

		void accept(StmtVisitor& visitor) const override {
			visitor.visit_block_stmt(*this);
		}
	};
}