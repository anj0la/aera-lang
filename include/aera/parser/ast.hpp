#pragma once

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <cstdint>
#include <variant>
#include <optional>

#include <aera/token/token.hpp>

namespace aera::parser {

	// Forward Declarations

	struct ASTNode;
	struct Program;
	struct Decl;
	struct FnDecl;
	struct VarDecl;
	struct ConstDecl;
	struct StructDecl;
	struct FieldDecl;
	struct ClassDecl;
	struct TraitDecl;
	struct WithDecl;
	struct Expr;
	struct Assignment;
	struct IdentifierLValue;
	struct ArrayAccessLValue;
	struct FieldAccessLValue;
	struct Conditional;
	struct Binary;
	struct Unary;
	struct ArrayAcces;
	struct FnCall;
	struct FieldAccess;
	struct Grouping;
	struct Literal;
	struct Identifier;
	struct Bind;
	struct Stmt;
	struct ExprStmt;
	struct IfStmt;
	struct WhileStmt;
	struct ForStmt;
	struct IteratorForStmt;
	struct RangeForStmt;
	struct LoopStmt;
	struct BlockStmt;

	// Types

	using Value = std::variant<int8_t, int16_t, int32_t, int64_t, 
		uint8_t, uint16_t, uint32_t, uint64_t, 
		float, double, std::string, char, bool>;

	using CppType = std::variant<int8_t, int16_t, int32_t, int64_t,
		uint8_t, uint16_t, uint32_t, uint64_t,
		float, double, std::string, char, bool>;

	enum class Type {
		// Integer
		Int8, Int16, Int32, Int64,
		Uint8, uint16, Uint32, Uint64,

		// Float
		Float32, Float64,

		// Others
		String, Character, Bool
	};

	// Result

	using Result = std::variant<>; // Will store the Type, and semantic analyzer, symbol table and cpp codegen structs

	// Visitor (Declarations)

	struct DeclVisitor {
		virtual ~DeclVisitor() = default;

		virtual Result visit_fn_decl(const FnDecl& decl) = 0;
		virtual Result visit_var_decl(const VarDecl& decl) = 0;
		virtual Result visit_const_decl(const ConstDecl& decl) = 0;
		virtual Result visit_struct_decl(const StructDecl& decl) = 0;
		virtual Result visit_class_decl(const ClassDecl& decl) = 0;
		virtual Result visit_field_decl(const FieldDecl& decl) = 0;
		virtual Result visit_trait_decl(const TraitDecl& decl) = 0;
		virtual Result visit_with_decl(const WithDecl& decl) = 0;
	};

	// Declarations

	struct Program {
		std::vector<Decl> decls;
	};

	struct Decl { // Abstract
		virtual Result accept(DeclVisitor& visitor) const = 0;
		virtual ~Decl() = default;
	};

	struct FnDecl : Decl { // Private visibility by default
		bool is_public = false;
		Token name;
		std::vector<std::pair<Token, Type>> params;
		std::optional<Type> return_type;
		std::vector<std::unique_ptr<Stmt>> body;

		Result accept(DeclVisitor& visitor) const override {
			return visitor.visit_fn_decl(*this);
		}
	};

	struct VarDecl : Decl { // Immutable by default, CAN be changed
		bool is_mutable = false;
		Token name;
		std::optional<Type> decl_type; // optionally declared by user
		std::optional<std::unique_ptr<Expr>> initializer;

		Result accept(DeclVisitor& visitor) const override {
			return visitor.visit_var_decl(*this);
		}
	};

	struct ConstDecl : Decl { // IMMUTABLE, cannot be changed
		Token name;
		Type type;
		std::unique_ptr<Expr> initializer;

		Result accept(DeclVisitor& visitor) const override {
			return visitor.visit_const_decl(*this);
		}
	}; 

	struct StructDecl : Decl {
		Token name;
		std::vector<std::unique_ptr<FieldDecl>> fdecls;

		Result accept(DeclVisitor& visitor) const override {
			return visitor.visit_struct_decl(*this);
		}
	};

	struct FieldDecl : Decl {
		Token name;
		Type type;
		std::unique_ptr<Expr> initializer;

		Result accept(DeclVisitor& visitor) const override {
			return visitor.visit_field_decl(*this);
		}
	};

	struct ClassDecl : Decl {
		Token name;
		std::vector<std::unique_ptr<FieldDecl>> fdecls;
		std::vector<std::unique_ptr<FnDecl>> fndecls;

		Result accept(DeclVisitor& visitor) const override {
			return visitor.visit_class_decl(*this);
		}
	};

	struct TraitDecl : Decl {
		Token name;
		std::vector<std::unique_ptr<FnDecl>> fndecls;

		Result accept(DeclVisitor& visitor) const override {
			return visitor.visit_trait_decl(*this);
		}
	};

	struct WithDecl : Decl {
		Token trait_name; // The trait
		Token type_name;  // The user-defined type to apply the trait to
		std::vector<FnDecl> fndecls;

		Result accept(DeclVisitor& visitor) const override {
			return visitor.visit_with_decl(*this);
		}
	};

	// Visitor (Expressions)

	struct ExprVisitor {
		virtual ~ExprVisitor() = default;

		virtual Result visit_assignment_expr(const Assignment& expr) = 0;
		virtual Result visit_conditional_expr(const Conditional& expr) = 0;
		virtual Result visit_binary_expr(const Binary& expr) = 0;
		virtual Result visit_unary_expr(const Unary& expr) = 0;
		virtual Result visit_array_access_expr(const ArrayAccess& expr) = 0;
		virtual Result visit_fn_call_expr(const FnCall& expr) = 0;
		virtual Result visit_field_access_expr(const FieldAccess& expr) = 0;
		virtual Result visit_grouping_expr(const Grouping& expr) = 0;
		virtual Result visit_literal_expr(const Literal& expr) = 0;
		virtual Result visit_identifier_expr(const Identifier& expr) = 0;
	};

	// Expressions

	struct Expr {
		virtual bool is_lvalue() const { 
			return false; 
		}
		virtual ~Expr() = default;
		virtual Result accept(ExprVisitor& visitor) const = 0;
	}; 

	struct Assignment : Expr {
		std::unique_ptr<Expr> expr;
		Token op;
		std::unique_ptr<Expr> rhs;

		Result accept(ExprVisitor& visitor) const override {
			return visitor.visit_assignment_expr(*this);
		}
	};

	struct Conditional : Expr {
		std::unique_ptr< Expr> conditional;
		std::unique_ptr< Expr> true_expr;
		std::unique_ptr< Expr> false_expr;

		Result accept(ExprVisitor& visitor) const override {
			return visitor.visit_conditional_expr(*this);
		}
	};

	struct Binary : Expr {
		std::unique_ptr< Expr> lhs;
		Token op;
		std::unique_ptr< Expr> rhs;

		Result accept(ExprVisitor& visitor) const override {
			return visitor.visit_binary_expr(*this);
		}
	};

	struct Unary : Expr {
		Token op;
		std::unique_ptr< Expr> rhs;

		Result accept(ExprVisitor& visitor) const override {
			return visitor.visit_unary_expr(*this);
		}
	};

	struct ArrayAccess : Expr {
		std::unique_ptr< Expr> expr;
		int64_t index;

		Result accept(ExprVisitor& visitor) const override {
			return visitor.visit_array_access_expr(*this);
		}

		bool is_lvalue() const override { 
			return true; 
		}
	};

	struct FnCall : Expr {
		std::unique_ptr< Expr> callee;
		Token paren;
		std::vector<std::unique_ptr< Expr>> args;

		Result accept(ExprVisitor& visitor) const override {
			return visitor.visit_fn_call_expr(*this);
		}
	};

	struct FieldAccess : Expr {
		std::unique_ptr< Expr> obj;
		Token name;

		Result accept(ExprVisitor& visitor) const override {
			return visitor.visit_field_access_expr(*this);
		}

		bool is_lvalue() const override { 
			return true; 
		}

	};

	struct Grouping : Expr {
		std::unique_ptr< Expr> expr;

		Result accept(ExprVisitor& visitor) const override {
			return visitor.visit_grouping_expr(*this);
		}
	};

	struct Literal : Expr {
		Value value;
		// Type type;
		// std::string raw_text; // would return typeinfo

		Result accept(ExprVisitor& visitor) const override {
			return visitor.visit_literal_expr(*this);
		}
	};

	struct Identifier : Expr {
		Token name;
		
		explicit Identifier(Token p_name) : name(p_name) {}

		Result accept(ExprVisitor& visitor) const override {
			return visitor.visit_identifier_expr(*this);
		}

		bool is_lvalue() const override { 
			return true; 
		}
	};

	// Visitor (Statements)
	
	struct StmtVisitor {
		virtual ~StmtVisitor() = default;
		virtual Result visit_expr_stmt(const ExprStmt& stmt) = 0;
		virtual Result visit_return_stmt(const ReturnStmt& stmt) = 0;
		virtual Result visit_if_stmt(const IfStmt& stmt) = 0;
		virtual Result visit_while_stmt(const WhileStmt& stmt) = 0;
		virtual Result visit_iterator_for_stmt(const IteratorForStmt& stmt) = 0;
		virtual Result visit_range_for_stmt(const RangeForStmt& stmt) = 0;
		virtual Result visit_loop_stmt(const LoopStmt& stmt) = 0;
		virtual Result visit_block_stmt(const BlockStmt& stmt) = 0;
	};

	// Statements

	struct Stmt { // Abstract
		virtual Result accept(StmtVisitor& visitor) const = 0;
		virtual ~Stmt() = default;
	}; 

	struct ExprStmt : Stmt {
		std::unique_ptr<Expr> expr;

		Result accept(StmtVisitor& visitor) const override {
			return visitor.visit_expr_stmt(*this);
		}
	};

	struct ReturnStmt : Stmt {
		Token keyword;
		std::unique_ptr<Expr> value;

		Result accept(StmtVisitor& visitor) const override {
			return visitor.visit_return_stmt(*this);
		}
	};

	struct IfStmt : Stmt {
		std::unique_ptr<Expr> condition;
		std::unique_ptr<Stmt> then_branch;
		std::unique_ptr<Stmt> else_branch;

		Result accept(StmtVisitor& visitor) const override {
			return visitor.visit_if_stmt(*this);
		}
	};

	struct WhileStmt : Stmt {
		std::unique_ptr<Expr> condition;
		std::unique_ptr<Stmt> body;

		Result accept(StmtVisitor& visitor) const override {
			return visitor.visit_while_stmt(*this);
		}
	};

	struct ForStmt : Stmt {
		Token name;
		std::unique_ptr<Stmt> body;
	};

	struct IteratorForStmt : ForStmt {
		std::unique_ptr<Expr> collection;

		Result accept(StmtVisitor& visitor) const override {
			return visitor.visit_iterator_for_stmt(*this);
		}
	};

	struct RangeForStmt : Stmt {
		std::unique_ptr<Expr> start_expr;
		std::unique_ptr<Expr> end_expr;

		Result accept(StmtVisitor& visitor) const override {
			return visitor.visit_range_for_stmt(*this);
		}
	};

	struct LoopStmt : Stmt {
		std::unique_ptr<Stmt> body;

		Result accept(StmtVisitor& visitor) const override {
			return visitor.visit_loop_stmt(*this);
		}
	};

	struct BlockStmt : Stmt {
		std::vector<std::unique_ptr<Stmt>> stmts;

		Result accept(StmtVisitor& visitor) const override {
			return visitor.visit_block_stmt(*this);
		}
	};
}