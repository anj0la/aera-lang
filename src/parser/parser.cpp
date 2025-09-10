#include <aera/parser/parser.hpp>

namespace aera::parser {

	std::vector<std::unique_ptr<Decl>> Parser::parse() {
		std::vector<std::unique_ptr<Decl>> decls;

		while (!is_at_end()) {
            auto decl = declaration(); // synchronizes on error
            if (decl != nullptr) {
                decls.push_back(std::move(decl));
            }
        }

        return decls;
	}

	std::unique_ptr<Decl> Parser::declaration() {
		if (check(TokenType::Pub) || check(TokenType::Fn)) {
			auto decl = fn_declaration();
			if (decl == nullptr) {
				sync();
				return nullptr;
			}
			return decl;
		}
		if (match(TokenType::Let)) {
			auto decl = var_declaration();
			if (decl == nullptr) {
				sync();
				return nullptr;
			}
			return decl;
		}
		if (match(TokenType::Const)) {
			auto decl = const_declaration();
			if (decl == nullptr) {
				sync();
				return nullptr;
			}
			return decl;
		}
		if (match(TokenType::Struct)) {
			auto decl = struct_declaration();
			if (decl == nullptr) {
				sync();
				return nullptr;
			}
			return decl;
		}
		if (match(TokenType::Class)) {
			auto decl = class_declaration();
			if (decl == nullptr) {
				sync();
				return nullptr;
			}
			return decl;
		}
		if (match(TokenType::Trait)) {
			auto decl = trait_declaration();
			if (decl == nullptr) {
				sync();
				return nullptr;
			}
			return decl;
		}
		if (match(TokenType::With)) {
			auto decl = with_declaration();
			if (decl == nullptr) {
				sync();
				return nullptr;
			}
			return decl;
		}
		error("couldn't parse declaration", "expected function, variable, user-defined, trait or with declaration");
		sync();
		return nullptr;
	}

	std::unique_ptr<Decl> Parser::fn_declaration() {
		std::optional<TokenType> visibility_modifier;
		if (match(TokenType::Pub)) {
			visibility_modifier = TokenType::Pub;
		}

		auto fn_token = consume(TokenType::Fn);
		if (!fn_token.has_value()) {
			error("expected 'fn' keyword");
			return nullptr;
		}

		auto name = consume(TokenType::Identifier);
		if (!name.has_value()) {
			error("expected function name after 'fn'");
			return nullptr;
		}

		auto left_paren = consume(TokenType::LeftParen);
		if (!left_paren.has_value()) {
			error("expected '(' after function name");
			return nullptr;
		}

		std::vector<std::pair<Token, std::unique_ptr<Type>>> params;

		skip_newlines(); // skip newlines after opening paren

		while (!check(TokenType::RightParen)) {
			auto param_name = consume(TokenType::Identifier);
			if (!param_name) {
				error("expected parameter name");
				return nullptr;
			}
			auto colon = consume(TokenType::Colon);
			if (!colon) {
				error("expected ':' after parameter name");
				return nullptr;
			}
			std::unique_ptr<Type> param_type = parse_type();
			if (!param_type) {
				return nullptr; // parse_type reports error
			}
			params.emplace_back(param_name.value(), std::move(param_type));

			skip_newlines(); // skip any newlines after parameter

			if (match(TokenType::Comma)) {
				skip_newlines(); // consume newlines after comma
			}
			else {
				break; // no comma, at closing paren (or error)
			}
		}

		auto right_paren = consume(TokenType::RightParen);
		if (!right_paren) {
			error("expected ')' after parameters");
			return nullptr;
		}

		std::unique_ptr<Type> return_type;

		if (match(TokenType::MinusGreater)) { // "->"
			return_type = parse_type();
			if (!return_type) {
				return nullptr; // parse_type reports error
			}
		}

		auto body = block();
		if (!body) {
			error("unable to parse function body");
			return nullptr; // block reports error
		}

		return std::make_unique<FnDecl>(
			name.value(),
			std::move(visibility_modifier),
			std::move(params),
			std::move(return_type),
			std::move(body)
		);
	}

	std::unique_ptr<Decl> Parser::var_declaration() {
		bool is_mutable = false;

		if (match(TokenType::Mut)) {
			is_mutable = true;
		}

		auto identifier = consume(TokenType::Identifier);
		if (!identifier.has_value()) {
			error("expected identifier name");
			return nullptr;
		}

		std::unique_ptr<Type> type_annotation; // optional (determined later if not defined)
		if (match(TokenType::Colon)) {
			type_annotation = parse_type(); 
			if (!type_annotation) {
				return nullptr; // parse_type reports error
			}
		}

		std::unique_ptr<Expr> initializer = nullptr; // optional 
		if (match(TokenType::Equal)) {
			initializer = expression();
			if (!initializer) {
				// error("expected expression after '='");
				return nullptr; // expression reports error
			}
		}

		if (!match(TokenType::Newline) && !match(TokenType::Semicolon)) {
			error("expected newline or ';' after variable declaration");
			return nullptr;
		}

		return std::make_unique<VarDecl>(
			identifier.value(),
			is_mutable,
			std::move(type_annotation),
			std::move(initializer)
		);
	}

	std::unique_ptr<Decl> Parser::const_declaration() {
		auto identifier = consume(TokenType::Identifier);
		if (!identifier.has_value()) {
			error("expected identifier name");
			return nullptr;
		}

		std::unique_ptr<Type> type_annotation; // optional
		if (match(TokenType::Colon)) {
			type_annotation = parse_type(); // parse_type handles errors
			if (!type_annotation) {
				return nullptr; // parse_type reports error
			}
		}

		auto equal_token = consume(TokenType::Equal);
		if (!equal_token.has_value()) {
			error("expected '=' for constant declaration", "constants must be initialized");
			return nullptr;
		}

		auto initializer = expression();
		if (!initializer) {
			error("expected expression after '='");
			return nullptr;
		}

		if (!match(TokenType::Newline) && !match(TokenType::Semicolon)) {
			error("expected newline or ';' after const variable declaration");
			return nullptr;
		}

		return std::make_unique<ConstDecl>(
			identifier.value(),
			std::move(type_annotation),
			std::move(initializer)
		);
	}

	std::unique_ptr<Decl> Parser::struct_declaration() {
		auto identifier = consume(TokenType::Identifier);
		if (!identifier.has_value()) {
			error("expected identifier name");
			return nullptr;
		}

		auto left_paren = consume(TokenType::LeftBrace);
		if (!left_paren.has_value()) {
			error("expected '{' after struct name");
			return nullptr;
		}

		std::vector<std::unique_ptr<FieldDecl>> fdecls;

		skip_newlines();
		while (!check(TokenType::RightBrace)) {
			auto fdecl = field_declaration();
			if (!fdecl) {
				return nullptr; // field_declaration reports error
			}
			fdecls.emplace_back(std::move(fdecl));
			skip_newlines();
		}

		auto right_brace = consume(TokenType::RightBrace);
		if (!right_brace) {
			error("expected '}' after field declarations");
			return nullptr;
		}

		return std::make_unique<StructDecl>(
			identifier.value(),
			std::move(fdecls)
		);
	}

	std::unique_ptr<Decl> Parser::field_declaration() {
		auto identifier = consume(TokenType::Identifier);
		if (!identifier.has_value()) {
			error("expected identifier name");
			return nullptr;
		}

		std::unique_ptr<Type> type_annotation;
		auto colon = consume(TokenType::Colon);
		if (!colon.has_value()) {
			error("expected type", "a field declaration must have an explicit type");
			return nullptr;
		}

		type_annotation = parse_type();
		if (!type_annotation) {
			return nullptr; // parse_type reports error
		}

		std::unique_ptr<Expr> initializer = nullptr;
		if (match(TokenType::Equal)) {
			initializer = expression();
			if (!initializer) {
				error("expected expression after '='");
				return nullptr;
			}
		}

		if (!match(TokenType::Newline) && !match(TokenType::Comma) && !match(TokenType::Semicolon)) {
			if (!check(TokenType::RightBrace)) {
				error("expected newline, ',' or ';' after field declaration");
				return nullptr;
			}
		}

		return std::make_unique<FieldDecl>(
			identifier.value(),
			std::move(type_annotation),
			std::move(initializer)
		);
	}

	std::unique_ptr<Decl> Parser::class_declaration() {
		auto class_name = consume(TokenType::Identifier);
		if (!class_name.has_value()) {
			error("expected class name");
			return nullptr;
		}

		std::optional<Token> parent_class;
		if (match(TokenType::Colon)) {
			parent_class = consume(TokenType::Identifier);
			if (!parent_class.has_value()) {
				error("expected parent class name after ':'");
				return nullptr;
			}
		}

		auto body = class_body();
		if (body.empty()) {
			return nullptr; // class_body reports error
		}

		return std::make_unique<ClassDecl>(
			class_name.value(),
			parent_class.value(),
			std::move(body)
		);
	}

	std::vector<ClassMember> Parser::class_body() {
		std::vector<ClassMember> members;

		auto left_brace = consume(TokenType::LeftBrace);
		if (!left_brace.has_value()) {
			error("expected '{' before class body");
			return {}; // return empty vector on error
		}

		while (!check(TokenType::RightBrace) && !is_at_end()) {
			if (check_field_declaration()) {
				auto field = field_declaration();
				if (field) {
					members.push_back(ClassMember::make_field(
						std::unique_ptr<FieldDecl>(static_cast<FieldDecl*>(field.release()))
					));
				}
				else {
					return {}; // field_declaration reports error
				}
			}
			else if (check_fn_declaration()) {
				auto fn = fn_declaration();
				if (fn) {
					members.push_back(ClassMember::make_function(
						std::unique_ptr<FnDecl>(static_cast<FnDecl*>(fn.release()))
					));
				}
				else {
					return {}; // fn_declaration reports error
				}
			}
			else {
				return {};
			}
		}

		auto right_brace = consume(TokenType::RightBrace);
		if (!right_brace.has_value()) {
			error("expected '}' after class body");
			return {};
		}

		return members;
	}

	std::unique_ptr<Decl> Parser::trait_declaration() {
		std::vector<std::unique_ptr<FnDecl>> fndecls;

		auto trait_name = consume(TokenType::Identifier);
		if (!trait_name.has_value()) {
			error("expected trait name");
			return nullptr;
		}

		auto left_brace = consume(TokenType::LeftBrace);
		if (!left_brace.has_value()) {
			error("expected '{' before trait body");
			return nullptr;
		}

		while (!check(TokenType::RightBrace) && !is_at_end()) {
			auto fn = fn_declaration();
			if (fn) {
				fndecls.emplace_back(std::move(fn));
			}
			else {
				return nullptr; // fn_declaration reports error
			}
		}

		auto right_brace = consume(TokenType::RightBrace);
		if (!right_brace.has_value()) {
			error("expected '}' after trait body");
			return nullptr;
		}

		return std::make_unique<TraitDecl>(
			trait_name.value(),
			std::move(fndecls)
		);

	}

	std::unique_ptr<Decl> Parser::with_declaration() {
		std::vector<std::unique_ptr<FnDecl>> fndecls;

		auto trait_name = consume(TokenType::Identifier);
		if (!trait_name.has_value()) {
			error("expected trait name");
			return nullptr;
		}

		auto for_token = consume(TokenType::For);
		if (!for_token.has_value()) {
			error("expected for keyword");
			return nullptr;
		}

		auto type_name = consume(TokenType::Identifier);
		if (!type_name.has_value()) {
			error("expected user-defined type name");
			return nullptr;
		}

		auto left_brace = consume(TokenType::LeftBrace);
		if (!left_brace.has_value()) {
			error("expected '{' before trait body");
			return nullptr;
		}

		while (!check(TokenType::RightBrace) && !is_at_end()) {
			auto fn = fn_declaration();
			if (fn) {
				fndecls.emplace_back(std::move(fn));
			}
			else {
				return nullptr; // fn_declaration reports error
			}
		}

		auto right_brace = consume(TokenType::RightBrace);
		if (!right_brace.has_value()) {
			error("expected '}' after trait body");
			return nullptr;
		}

		return std::make_unique<WithDecl>(
			trait_name.value(),
			type_name.value(),
			std::move(fndecls)
		);
	}

	// Expressions

	std::unique_ptr<Expr>  Parser::expression() {
		return assignment();
	}

	std::unique_ptr<Expr> Parser::assignment() {
		auto expr = conditional();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		if (match_assignment_op()) {
			if (!expr->is_lvalue()) {
				error("cannot assign to this expression - not a valid assignment target");
				return nullptr;
			}

			Token op = prev();
			auto rhs = assignment();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			return std::make_unique<AssignmentExpr>(std::move(expr), op, std::move(rhs));
		}
		return expr;
	}

	std::unique_ptr<Expr> Parser::conditional() {
		auto expr = logical_or();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		if (match(TokenType::If)) {
			auto condition = expression();

			if (!condition) {
				return nullptr; // error already reported, propagate
			}

			auto token = consume(TokenType::Else);

			if (!token.has_value()) {
				auto else_expr = conditional();

				if (!else_expr) {
					return nullptr; // error already reported, propagate
				}

				return std::make_unique<ConditionalExpr>(std::move(expr),
					std::move(condition),
					std::move(else_expr));
			}
			else {
				error("expected 'else' after conditional expression");
				return nullptr; // invalid expression
			}
		}
		return expr;
	}

	std::unique_ptr<Expr> Parser::logical_or() {
		auto expr = logical_and();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (match(TokenType::PipePipe)) {
			Token op = prev();
			auto rhs = logical_and();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(rhs));
		}
		return expr;
	}

	std::unique_ptr<Expr> Parser::logical_and() {
		auto expr = bitwise_or();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (match(TokenType::AmpAmp)) {
			Token op = prev();
			auto rhs = bitwise_or();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(rhs));
		}
		return expr;
	}

	std::unique_ptr<Expr> Parser::bitwise_or() {
		auto expr = bitwise_xor();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (match(TokenType::Pipe)) {
			Token op = prev();
			auto rhs = bitwise_xor();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::bitwise_xor() {
		auto expr = bitwise_and();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (match(TokenType::Caret)) {
			Token op = prev();
			auto rhs = bitwise_and();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::bitwise_and() {
		auto expr = equality();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (match(TokenType::Amp)) {
			Token op = prev();
			auto rhs = equality();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::equality() {
		auto expr = comparison();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (match(TokenType::EqualEqual, TokenType::ExclaimEqual)) {
			Token op = prev();
			auto rhs = comparison();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::comparison() {
		auto expr = shift();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (match(TokenType::Greater, TokenType::GreaterEqual, TokenType::Less, TokenType::LessEqual)) {
			Token op = prev();
			auto rhs = shift();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::shift() {
		auto expr = term();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (match(TokenType::GreaterGreater, TokenType::LessLess)) {
			Token op = prev();
			auto rhs = term();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::term() {
		auto expr = factor();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (match(TokenType::Plus, TokenType::Minus)) {
			Token op = prev();
			auto rhs = factor();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::factor() {
		auto expr = unary();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (match(TokenType::Star, TokenType::Slash)) {
			Token op = prev();
			auto rhs = unary();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			expr = std::make_unique<BinaryExpr>(std::move(expr), op, std::move(rhs));
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::unary() {
		if (match(TokenType::Exclaim, TokenType::Minus, TokenType::Tilde, TokenType::Amp)) {
			Token op = prev();
			auto rhs = unary();

			if (!rhs) {
				return nullptr; // error already reported, propagate
			}

			return std::make_unique<UnaryExpr>(op, std::move(rhs));
		}

		return cast();
	}
	std::unique_ptr<Expr> Parser::cast() {
		auto expr = postfix();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		if (match(TokenType::As)) {
			auto type = parse_type();
			if (!type) {
				return nullptr; // parse_type reports error
			}
			return std::make_unique<CastExpr>(std::move(expr), type);
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::postfix() {
		auto expr = primary();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		while (true) {
			// Array access
			if (match(TokenType::LeftBracket)) {
				auto index = expression();

				if (!index) {
					return nullptr; // error already reported, propagate
				}

				auto token = consume(TokenType::RightBracket);
				if (token.has_value()) {
					expr = std::make_unique<ArrayAccess>(std::move(expr), std::move(index));
				}
				else {
					error("expected ']' after array index");
					return nullptr;
				}
			}
			// Field access
			else if (match(TokenType::Period)) {
				auto name = consume(TokenType::Identifier);
				if (name.has_value()) {
					expr = std::make_unique<FieldAccessExpr>(std::move(expr), name);
				}
				else {
					error("expected property name after '.'");
					return nullptr;
				}
			}
			// Arguments
			else if (match(TokenType::LeftParen)) {
				std::vector<std::unique_ptr<Expr>> args;
				if (!check(TokenType::RightParen)) {
					args = argument_list();
					if (args.empty()) {
						return nullptr; // argument_list reports error
					}
				}

				auto token = consume(TokenType::RightParen);
				if (token.has_value()) {
					expr = std::make_unique<FnCallExpr>(std::move(expr), std::move(args));
				}
				else {
					error("expected ')' after arguments");
					return nullptr;
				}
			}
			else if (match(TokenType::Question)) {
				expr = std::make_unique<UnwrapExpr>(std::move(expr));
			}
			else {
				break;
			}
		}
		return expr;
	}

	std::unique_ptr<Expr> Parser::primary() {
		// Literals
		if (match(TokenType::IntLiteral, TokenType::FloatLiteral,
			TokenType::CharacterLiteral, TokenType::StringLiteral)) {
			return std::make_unique<LiteralExpr>(prev().value);
		}
		if (match(TokenType::True)) {
			return std::make_unique<LiteralExpr>(true);
		}
		if (match(TokenType::False)) {
			return std::make_unique<LiteralExpr>(false);
		}

		// Identifier
		if (match(TokenType::Identifier)) {
			return std::make_unique<IdentifierExpr>(prev());
		}

		// Grouping
		if (match(TokenType::LeftParen)) {
			auto expr = expression();

			if (!expr) {
				return nullptr; // error already reported, propagate
			}

			auto token = consume(TokenType::RightParen);
			if (token.has_value()) {
				return std::make_unique<GroupingExpr>(std::move(expr));
			}
			else {
				error("expected ')' after expression");
				return nullptr;
			}
		}

		error("couldn't parse literal", "expected an expression");
		return nullptr;
	}

	std::vector<std::unique_ptr<Expr>> Parser::argument_list() {
		std::vector<std::unique_ptr<Expr>> args;
		auto expr = expression();

		if (!expr) {
			return {}; // expression reports error
		}
		args.emplace_back(std::move(expr));

		while (match(TokenType::Comma)) {
			auto expr = expression();

			if (!expr) {
				return {}; // expression reports error
			}
			args.emplace_back(std::move(expr));
		}

		return args;
	}

	// Statements

	std::unique_ptr<Stmt> Parser::statement() {
		if (match(TokenType::Fn, TokenType::Let, TokenType::Const, TokenType::Struct,
			TokenType::Class, TokenType::Trait, TokenType::With)) {
			return declaration();
		}

		if (match(TokenType::Return)) {
			return return_statement();
		}
		if (match(TokenType::If)) {
			return if_statement();
		}
		if (match(TokenType::While)) {
			return while_statement();
		}
		if (match(TokenType::For)) {
			return for_statement();
		}
		if (match(TokenType::Loop)) {
			return loop_statement();
		}
		if (match(TokenType::Match)) {
			return match_statement();
		}
		if (match(TokenType::LeftBrace)) {
			return block();
		}

		return expression_statement();
	}

	std::unique_ptr<Stmt> Parser::expression_statement() {
		auto expr = expression();

		if (!expr) {
			return nullptr; // error already reported, propagate
		}

		if (!match(TokenType::Newline) && !match(TokenType::Semicolon)) {
			error("expected newline or ';' after expression");
			return nullptr;
		}

		return std::make_unique<ExprStmt>(std::move(expr));
	}

	std::unique_ptr<Stmt> Parser::return_statement() {
		Token keyword = prev();
		std::unique_ptr<Expr> expr;

		if (!check(TokenType::Semicolon) && !check(TokenType::Newline)) { // for examples like return 5, return 5; return\n 5
			expr = expression();
			if (!expr) {
				return nullptr; // error already reported, propagate
			}
		}

		if (!match(TokenType::Newline) && !match(TokenType::Semicolon)) { // checks for return\n / return;
			error("expected newline or ';' after return statement");
			return nullptr;
		}

		return std::make_unique<ReturnStmt>(keyword, std::move(expr));
	}

	std::unique_ptr<Stmt> Parser::if_statement() {
		std::unique_ptr<Expr> condition = expression();
		std::unique_ptr<Stmt> then_branch = statement();
		std::unique_ptr<Stmt> else_branch;

		if (!condition || !then_branch) {
			return nullptr; // error already reported, propagate
		}

		if (match(TokenType::Else)) {
			else_branch = statement();
			if (!else_branch) {
				return nullptr; // error already reported, propagate
			}
		}

		return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
	}

	std::unique_ptr<Stmt> Parser::while_statement() {
		std::unique_ptr<Expr> condition = expression();
		std::unique_ptr<Stmt> body = statement();

		if (!condition || !body) {
			return nullptr; // error already reported, propagate
		}

		return std::make_unique<WhileStmt>(
			std::move(condition), 
			std::move(body)
		);
	}

	std::unique_ptr<Stmt> Parser::for_statement() {
		auto identifier = consume(TokenType::Identifier);
		if (!identifier.has_value()) {
			error("expected identifier after 'for'");
			return nullptr;
		}

		auto in_token = consume(TokenType::In);
		if (!in_token.has_value()) {
			error("expected 'in' after identifier");
			return nullptr;
		}

		auto start_expr = expression();

		if (start_expr) {
			return nullptr; // error already reported, propagate
		}

		if (match(TokenType::PeriodPeriod, TokenType::PeriodPeriodEqual)) {
			// Range for loop
			auto end_expr = expression();
			auto body = statement();

			if (!end_expr || !body) {
				return nullptr; // error already reported, propagate
			}

			return std::make_unique<RangeForStmt>(
				identifier, 
				start_expr, 
				end_expr, 
				body
			);
		}
		else {
			// Iterator for loop  
			auto body = statement();

			if (body) {
				return nullptr; // error already reported, propagate
			}

			return std::make_unique<IteratorForStmt>(identifier, start_expr, body);
		}
	}

	std::unique_ptr<Stmt> Parser::loop_statement() {
		std::unique_ptr<Stmt> body = statement();

		if (body) {
			return nullptr; // error already reported, propagate
		}

		return std::make_unique<LoopStmt>(std::move(body));
	}

	std::unique_ptr<Stmt> Parser::match_statement() {
		auto expr = expression();
		

		if (!expr) {
			return nullptr; // expression reports error
		}

		auto left_brace = consume(TokenType::LeftBrace);
		if (!left_brace.has_value()) {
			error("expected '{' after match expression");
			return nullptr;
		}

		std::vector<MatchClause> clauses;

		while (!check(TokenType::RightBrace) && !is_at_end()) {
			auto clause = match_clause();
			if (!clause.has_value()) {
				return nullptr; // match_clause reports error
			}
			clauses.push_back(std::move(clause.value()));

			if (!check(TokenType::RightBrace) && !is_at_end()) {
				auto comma = consume(TokenType::Comma);
				if (!comma.has_value()) {
					error("expected ',' after match clause");
					return nullptr;
				}
			}

			skip_newlines();
		}

		auto right_brace = consume(TokenType::RightBrace);
		if (!right_brace.has_value()) {
			error("expected '}' after match clauses");
			return nullptr;
		}

		return std::make_unique<MatchStmt>(std::move(expr), std::move(clauses));
	}

	std::optional<MatchClause> Parser::match_clause() {
		auto pattern = expression();
		if (!pattern) {
			return std::nullopt; // expression reports error
		}

		auto arrow = consume(TokenType::EqualGreater); // "=>"
		if (!arrow.has_value()) {
			error("expected '=>' after pattern");
			return std::nullopt;
		}

		auto expr = expression();
		if (!expr) {
			return std::nullopt; // expression reports error
		}

		return MatchClause{ std::move(pattern), std::move(expr) };
	}

	std::unique_ptr<BlockStmt> Parser::block() {
		std::vector<std::unique_ptr<Stmt>> stmts;

		auto left_brace = consume(TokenType::LeftBrace);
		if (!left_brace.has_value()) {
			error("expected '{' before block", "a block must always have an opening '{'");
			return nullptr;
		}

		while (!check(TokenType::RightBrace) && !is_at_end()) {
			auto stmt = statement();

			if (!stmt) {
				return nullptr; // error already reported, propagate
			}

			stmts.emplace_back(std::move(stmt));
		}

		auto token = consume(TokenType::RightBrace);
		if (token.has_value()) {
			return std::make_unique<BlockStmt>(std::move(stmts));
		}
		else {
			error("expected '}' after block", "a block must always have a closing '}' for every opening '{'");
			return nullptr;
		}
	}

	// UPDATE TYPES

	std::unique_ptr<Type> Parser::parse_type() {

		auto base_type = parse_base_type();
		if (!base_type) {
			return nullptr; // parse_base_type reports error
		}

		if (check(TokenType::LeftBracket)) {
			auto dimensions = parse_array_dimensions();
			if (dimensions.empty()) {
				return nullptr; // parse_array_dimensions reports error
			}
			return std::make_unique<StaticArrayType>(std::move(base_type), std::move(dimensions));
		}

		return base_type;
	}

	std::unique_ptr<Type> Parser::parse_base_type() {
		if (check(TokenType::Identifier)) {
			std::string name = peek().lexeme;

			// Check for primitive types
			if (is_primitive_type(name)) {
				advance();
				return parse_primitive_type(name);
			}

			// Check for built-in types
			if (name == "arr" || name == "map" || name == "set" || name == "opt" || name == "res") {
				advance();
				if (!match(TokenType::Exclaim)) {
					error("expected '!' after " + name);
					return nullptr;
				}
				if (!match(TokenType::Less)) {
					error("expected '<' after " + name + "!");
					return nullptr;
				}
				skip_newlines(); // allow newlines after 
				return parse_builtin_type(name);
			}

			// Check for generic user type (identifier!<...>)
			advance(); // consume identifier
			if (match(TokenType::Exclaim)) {
				if (!match(TokenType::Less)) {
					error("expected '<' after '!' for generic type");
					return nullptr;
				}
				skip_newlines(); // allow newlines after 
				return parse_generic_type(name);
			}

			// Regular user type
			return parse_user_type(name);
		}

		error("expected type name");
		return nullptr;
	}

	std::unique_ptr<Type> Parser::parse_primitive_type(const std::string& name) {
		auto prim_type = std::make_unique<PrimitiveType>();

		if (name == "int8") {
			prim_type->kind = PrimitiveTypeKind::Int8;
		}
		else if (name == "int16") {
			prim_type->kind = PrimitiveTypeKind::Int16;
		}
		else if (name == "int32") {
			prim_type->kind = PrimitiveTypeKind::Int32;
		}
		else if (name == "int64") {
			prim_type->kind = PrimitiveTypeKind::Int64;
		}
		else if (name == "uint8") {
			prim_type->kind = PrimitiveTypeKind::Uint8;
		}
		else if (name == "uint16") {
			prim_type->kind = PrimitiveTypeKind::Uint16;
		}
		else if (name == "uint32") {
			prim_type->kind = PrimitiveTypeKind::Uint32;
		}
		else if (name == "uint64") {
			prim_type->kind = PrimitiveTypeKind::Uint64;
		}
		else if (name == "float32") {
			prim_type->kind = PrimitiveTypeKind::Float32;
		}
		else if (name == "float64") {
			prim_type->kind = PrimitiveTypeKind::Float64;
		}
		else if (name == "string") {
			prim_type->kind = PrimitiveTypeKind::String;
		}
		else if (name == "char") {
			prim_type->kind = PrimitiveTypeKind::Character;
		}
		else if (name == "bool") {
			prim_type->kind = PrimitiveTypeKind::Bool;
		}

		return prim_type;
	}

	std::unique_ptr<Type> Parser::parse_builtin_type(const std::string& type_name) {
		if (type_name == "arr") {
			auto element_type = parse_type();
			if (!element_type) {
				return nullptr; // parse_type reports error
			}

			skip_newlines(); // allow newlines after 
			if (!consume(TokenType::Greater).has_value()) {
				error("expected '>' to close arr type");
				return nullptr;
			}

			return std::make_unique<DynamicArrayType>(std::move(element_type));
		}
		else if (type_name == "set") {
			auto element_type = parse_type();
			if (!element_type) {
				return nullptr; // parse_type reports error
			}

			skip_newlines(); // allow newlines after 
			if (!consume(TokenType::Greater).has_value()) {
				error("expected '>' to close set type");
				return nullptr;
			}

			return std::make_unique<SetType>(std::move(element_type));
		}
		else if (type_name == "map") {
			auto key_type = parse_type();
			if (!key_type) {
				return nullptr; // parse_type reports error
			}

			skip_newlines(); // allow newlines before comma
			if (!consume(TokenType::Comma).has_value()) {
				error("expected ',' in map type");
				return nullptr;
			}

			skip_newlines(); // allow newlines after commma
			auto value_type = parse_type();
			if (!value_type) { 
				return nullptr; // parse_type reports error
			}

			skip_newlines(); // allow newlines before closing >
			if (!consume(TokenType::Greater).has_value()) {
				error("expected '>' to close map type");
				return nullptr;
			}

			return std::make_unique<MapType>(std::move(key_type), std::move(value_type));
		}
		else if (type_name == "opt") {
			auto wrapped_type = parse_type();
			if (!wrapped_type) {
				return nullptr; // parse_type reports error
			}	

			skip_newlines(); // allow newlines after 
			if (!consume(TokenType::Greater).has_value()) {
				error("expected '>' to close opt type");
				return nullptr;
			}

			return std::make_unique<OptionalType>(std::move(wrapped_type));
		}
		else if (type_name == "res") {
			auto success_type = parse_type();
			if (!success_type) {
				return nullptr; // parse_type reports error
			}

			skip_newlines(); // allow newlines before comma
			if (!consume(TokenType::Comma).has_value()) {
				error("expected ',' in res type");
				return nullptr;
			}

			skip_newlines(); // allow newlines after commma
			auto error_type = parse_type();
			if (!error_type) {
				return nullptr; // parse_type reports error
			}

			skip_newlines(); // allow newlines before closing >
			if (!consume(TokenType::Greater).has_value()) {
				error("expected '>' to close res type");
				return nullptr;
			}

			return std::make_unique<ResultType>(std::move(success_type),std::move(error_type));
		}

		error("unknown builtin type: " + type_name);
		return nullptr;
	}

	std::unique_ptr<Type> Parser::parse_user_type(const std::string& name) {
		auto user_type = std::make_unique<UserType>();
		user_type->name = name;
		return user_type;
	}

	std::unique_ptr<Type> Parser::parse_generic_type(const std::string& name) {
		std::vector<std::unique_ptr<Type>> type_arguments;

		auto first_arg = parse_type();
		if (!first_arg) {
			return nullptr; // parse_type reports error
		}
		type_arguments.emplace_back(std::move(first_arg));

		while (match(TokenType::Comma)) {
			skip_newlines(); // allow newlines after comma
			auto arg = parse_type();
			if (!arg) {
				return nullptr; // parse_type reports error
			}
			type_arguments.emplace_back(std::move(arg));
		}

		skip_newlines(); // allow newlines before closing >
		if (!consume(TokenType::Greater).has_value()) {
			error("expected '>' to close generic type");
			return nullptr;
		}

		auto generic_type = std::make_unique<GenericType>();
		generic_type->name = name;
		generic_type->type_arguments = std::move(type_arguments);
		return generic_type;
	}

	std::vector<int> Parser::parse_array_dimensions() {
		std::vector<int> dimensions;

		while (peek().type == TokenType::LeftBracket) {
			advance(); // consume '['
			skip_newlines(); // allow newlines after [

			if (peek().type == TokenType::IntLiteral) {
				dimensions.push_back(std::get<int>(peek().value));
				advance();
			}
			else { // invalid content inside brackets
				error("expected integer literal in array dimension");
				return {};
			}

			skip_newlines(); // allow newlines before ]
			auto right_bracket = consume(TokenType::RightBracket);
			if (!right_bracket.has_value()) {
				error("expected ']' to close array dimension");
				return {};
			}
		}

		return dimensions;
	}

	bool Parser::is_primitive_type(const std::string& name) const {
		return name == "int8" || name == "int16" || name == "int32" || name == "int64" ||
			name == "uint8" || name == "uint16" || name == "uint32" || name == "uint64" ||
			name == "float32" || name == "float64" || name == "string" || name == "char" || name == "bool";
	}

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

	bool Parser::check_next(TokenType type) {
		if (is_at_end()) {
			return false;
		}

		return peek_next().type == type;
	}

	std::optional<Token> Parser::consume(TokenType type) {
		if (check(type)) {
			return advance();
		}

		return std::nullopt;
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

	Token Parser::peek_next() {
		return tokens[current + 1];
	}

	Token Parser::prev() {
		return tokens[current - 1];
	}

	void Parser::sync() {
		advance();

		while (!is_at_end()) {
			if (prev().type == TokenType::Semicolon || prev().type == TokenType::Newline) { // need to handle newline in parser
				return;
			}
		}

		switch (peek().type) {
			// Declarations
		case TokenType::Fn:
		case TokenType::Let:
		case TokenType::Const:
		case TokenType::Struct:
		case TokenType::Class:
		case TokenType::Trait:
		case TokenType::With:
			// Statements
		case TokenType::Return:
		case TokenType::If:
		case TokenType::While:
		case TokenType::For:
		case TokenType::Loop:
			return;
			break;
		}
		advance();
	}

	bool Parser::check_field_declaration() {
		return check(TokenType::Identifier) && check_next(TokenType::Colon);
	}

	bool Parser::check_fn_declaration() {
		return check(TokenType::Fn) ||
			(check(TokenType::Pub) && check_next(TokenType::Fn));
	}

	void Parser::error(const std::string& msg, const std::string& note) {
		Token curr = peek();
		reporter_.add_error(source_context_.filename(), curr.lexeme.length(), curr.loc, msg, std::string(source_context_.get_line(curr.loc.line)), note);
	}

	void Parser::skip_newlines() {
		while (match(TokenType::Newline)) {
			// consume newlines
		}
	}

}