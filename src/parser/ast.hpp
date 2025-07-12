#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <variant>
#include <optional>

#include <token.hpp>

namespace aera::parser {

	using IntValue = std::variant<int8_t, int16_t, int32_t, int64_t, uint8_t, uint16_t, uint32_t, uint64_t>;
	using FloatValue = std::variant<float, double>;
	using LiteralValue = std::variant<std::string, char, bool, IntValue, FloatValue>;

	enum class Type {
		// Integer
		Int8, Int16, Int32, Int64,
		Uint8, uint16, Uint32, Uint64,

		// Float
		Float32, Float64,

		// Others
		String, Character, Bool
	};

	// Base node

	struct ASTNode {
		int line = 1;
		int col = 1;
	};

	// Declarations

	struct Program : ASTNode {
		std::vector<Decl> decls;
	};

	struct Decl : ASTNode { // Abstract

		template<typename T>
		struct Visitor {
			virtual ~Visitor() = default;
			virtual T visit_fn_decl(const FnDecl& decl) = 0;
			virtual T visit_var_decl(const VarDecl& decl) = 0;
			virtual T visit_const_decl(const ConstDecl& decl) = 0;
			virtual T visit_struct_decl(const ClassDecl& decl) = 0;
			virtual T visit_field_decl(const FieldDecl& decl) = 0;
			virtual T visit_class_decl(const ClassDecl& decl) = 0;
			virtual T visit_interface_decl(const ClassDecl& decl) = 0;
		};

		template<typename T>
		virtual T accept(Visitor<T>& visitor) const = 0;

		virtual ~Decl() = default;
	
	};

	struct FnDecl : Decl { // Private visibility by default
		bool is_public = false;
		Token name;
		std::vector<Token> params;
		std::optional<Type> return_type;
		std::vector<Stmt> body;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_fn_decl(*this);
		}
	};

	struct VarDecl : Decl { // Immutable by default, CAN be changed
		bool is_mutable = false;
		Token name;
		std::optional<Type> decl_type; // optionally declared by user
		Type infer_type;			   // inferred type
		Expr initializer;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_var_decl(*this);
		}
	};

	struct ConstDecl : Decl { // IMMUTABLE, cannot be changed
		Token name;
		std::optional<Type> decl_type; // optionally declared by user
		Type infer_type;			   // inferred type
		Expr initializer;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_const_decl(*this);
		}

	}; 

	struct StructDecl : Decl {
		Token name;
		std::vector<FieldDecl> fdecls;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_struct_decl(*this);
		}
	};

	struct FieldDecl : Decl{
		Token name;
		Type type;			   // inferred type
		Expr initializer;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_field_decl(*this);
		}
	}

	struct ClassDecl : Decl {
		Token name;
		std::vector<FieldDecl> fdecls;
		std::vector<FnDecl> fndecls;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_class_decl(*this);
		}
	};

	struct InterfaceDecl : Decl {
		Token name;
		std::vector<FnDecl> fndecls;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_interface_decl(*this);
		}
	};

	// Expressions

	struct Expr : ASTNode { // Abstract

		template<typename T>
		struct Visitor {
			virtual ~Visitor() = default;
			virtual T visit_assignment_expr(const Assignment& expr) = 0;
			virtual T visit_identifier_lvalue_expr(const IdentifierLValue& expr) = 0;
			virtual T visit_array_access_lvalue_expr(const ArrayAccessLValue& expr) = 0;
			virtual T visit_field_access_lvalue_expr(const FieldAccessLValue& expr) = 0;
			virtual T visit_conditional_expr(const Conditional& expr) = 0;
			virtual T visit_binary_expr(const Binary& expr) = 0;
			virtual T visit_unary_expr(const Unary& expr) = 0;
			virtual T visit_array_access_expr(const ArrayAccess& expr) = 0;
			virtual T visit_fn_call_expr(const FnCall& expr) = 0;
			virtual T visit_member_access_expr(const MemberAccess& expr) = 0;
			virtual T visit_grouping_expr(const Grouping& expr) = 0;
			virtual T visit_literal_expr(const Literal& expr) = 0;
			virtual T visit_identifier_expr(const Identifier& expr) = 0;
			virtual T visit_unsafe_expr(const Unsafe& expr) = 0;
		};

		template<typename T>
		virtual T accept(Visitor<T>& visitor) const = 0;

		virtual ~Expr() = default;

	}; 

	struct Assignment : Expr {
		LValue lvalue;
		Token op;
		Expr rhs;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_assignment_expr(*this);
		}
	};

	struct LValue : Expr {};

	struct IdentifierLValue : LValue {
		Token name;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_identifier_lvalue_expr(*this);
		}
	};

	struct ArrayAccessLValue : LValue {
		LValue lvalue;
		int64_t index;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_array_access_lvalue_expr(*this);
		}
	};

	struct FieldAccessLValue : LValue {
		LValue lvalue;
		Token name;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_field_access_lvalue_expr(*this);
		}
	};

	struct Conditional : Expr {
		Expr conditional;
		Expr true_expr;
		Expr false_expr;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_conditional_expr(*this);
		}
	};

	struct Binary : Expr {
		Expr lhs;
		Token op;
		Expr rhs;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_binary_expr(*this);
		}
	};

	struct Unary : Expr {
		Token op;
		Expr rhs;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_unary_expr(*this);
		}
	};

	struct ArrayAccess : Expr {
		Expr expr;
		int64_t index;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_array_access_expr(*this);
		}
	};

	struct FnCall : Expr {
		Expr callee;
		Token paren;
		std::vector<Expr> arguments;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_fn_call_expr(*this);
		}
	};

	struct MemberAccess : Expr {
		Expr obj;
		Token name;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_member_access_expr(*this);
		}
	};

	struct Grouping : Expr {
		Expr expr;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_grouping_expr(*this);
		}
	};

	struct Literal : Expr {
		LiteralValue value;
		Type type;
		std::string raw_text;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_literal_expr(*this);
		}
	};

	struct Identifier : Expr {
		Token name;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_identifier_expr(*this);
		}
	};

	struct Unsafe : Expr {
		Stmt stmt;
		std::optional<Expr> expr;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_unsafe_expr(*this);
		}
	};

	// Statements

	struct Stmt : ASTNode { // Abstract

		template<typename T>
		struct Visitor {
			virtual ~Visitor() = default;
			virtual T visit_expr_stmt(const ExprStmt& stmt) = 0;
			virtual T visit_return_stmt(const ReturnStmt& stmt) = 0;
			virtual T visit_if_stmt(const IfStmt& stmt) = 0;
			virtual T visit_while_stmt(const WhileStmt& stmt) = 0;
			virtual T visit_iterator_for_stmt(const IteratorForStmt& stmt) = 0;
			virtual T visit_range_for_stmt(const RangeForStmt& stmt) = 0;
			virtual T visit_loop_stmt(const LoopStmt& stmt) = 0;
			virtual T visit_block_stmt(const BlockStmt& stmt) = 0;
		};

		template<typename T>
		virtual T accept(Visitor<T>& visitor) const = 0;

		virtual ~Stmt() = default;
	
	}; 

	struct ExprStmt : Stmt {
		Expr expr;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_expr_stmt(*this);
		}
	};

	struct ReturnStmt : Stmt {
		Token keyword;
		Expr value;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_return_stmt(*this);
		}
	};

	struct IfStmt : Stmt {
		Expr condition;
		Stmt then_branch;
		Stmt else_branch;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_if_stmt(*this);
		}
	};

	struct WhileStmt : Stmt {
		Expr condition;
		Stmt body;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_while_stmt(*this);
		}
	};

	struct ForStmt : Stmt {
		Token name;
		Stmt body;
	};

	struct IteratorForStmt : ForStmt {
		Expr collection;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_iterator_for_stmt(*this);
		}
	};

	struct RangeForStmt : Stmt {
		Expr start_expr;
		Expr end_expr;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_range_for_stmt(*this);
		}
	};

	struct LoopStmt : Stmt {
		Stmt body;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_loop_stmt(*this);
		}
	};

	struct BlockStmt : Stmt {
		std::vector<Stmt> stmts;

		template<typename T>
		T accept(Visitor<T>& visitor) const override {
			return visitor.visit_block_stmt(*this);
		}
	};
}