#include <aera/parser/parser.hpp>

namespace aera::parser {

	std::vector<std::unique_ptr<Decl>> Parser::parse() {
		std::vector<std::unique_ptr<Decl>> decls;

		while (!is_at_end()) {
			decls.push_back(std::move(declaration()));
		}

		return decls;
	}

	std::unique_ptr<Decl> Parser::declaration() {
		if (check(TokenType::At) || check(TokenType::Pub) || check(TokenType::Modifies) || check(TokenType::Fn)) {
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
		std::vector<Token> decorators;
		while (match(TokenType::At)) {
			auto decorator_name = consume(TokenType::Identifier);
			if (!decorator_name.has_value()) {
				error("expected decorator name after '@'");
				return nullptr;
			}
			decorators.push_back(decorator_name.value());
		}

		std::optional<TokenType> visibility_modifier;
		if (match(TokenType::Pub)) {
			visibility_modifier = TokenType::Pub;
		}

		std::optional<TokenType> behaviour_modifier;
		if (match(TokenType::Modifies)) {
			behaviour_modifier = TokenType::Modifies;
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
		if (!check(TokenType::RightParen)) {
			do {
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
					return nullptr;
				}
				params.emplace_back(param_name.value(), std::move(param_type));
			} while (match(TokenType::Comma));
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
				return nullptr;
			}
		}

		auto body = block();
		if (!body) {
			error("unable to parse function body");
			return nullptr;
		}

		return std::make_unique<FnDecl>(
			name.value(),
			std::move(decorators),
			std::move(visibility_modifier),
			std::move(behaviour_modifier),
			std::move(params),
			std::move(return_type),
			std::move(body)
		);
	}

	std::unique_ptr<Decl> Parser::var_declaration() {
		bool is_mutable = false;

		// Match optional 'mut' keyword to make variable mutable
		if (match(TokenType::Mut)) {
			is_mutable = true;
		}

		auto identifier = consume(TokenType::Identifier);
		if (!identifier.has_value()) {
			error("expected identifier name");
			return nullptr;
		}

		// Optional type annotation
		std::unique_ptr<Type> type_annotation;
		if (match(TokenType::Colon)) {
			type_annotation = parse_type(); // parse_type handles errors
			if (!type_annotation) {
				return nullptr;
			}
		}

		// Optional initializer
		std::unique_ptr<Expr> initializer = nullptr;
		if (match(TokenType::Equal)) {
			initializer = expression();
			if (!initializer) {
				error("expected expression after '='");
				return nullptr;
			}
		}

		auto semicolon = consume(TokenType::Semicolon);
		if (!semicolon.has_value()) {
			error("expected ';' after variable declaration", "a declaration must end with a semicolon char ';'");
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

		// Optional type annotation
		std::unique_ptr<Type> type_annotation;
		if (match(TokenType::Colon)) {
			type_annotation = parse_type(); // parse_type handles errors
			if (!type_annotation) {
				return nullptr;
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

		auto semicolon = consume(TokenType::Semicolon); // WE NEED TO CHANGE THIS
		if (!semicolon.has_value()) {
			error("expected ';' after variable declaration", "a declaration must end with a semicolon char ';'");
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

		while (!check(TokenType::RightBrace)) {
			auto fdecl = field_declaration();
			if (!fdecl) {
				error("unable to parse field declaration");
				return nullptr;
			}
			fdecls.emplace_back(std::move(fdecl));
		}

		auto right_brace = consume(TokenType::RightBrace);
		if (!right_brace) {
			error("expected '}' after field declarations");
			return nullptr;
		}

		return std::make_unique<StructDecl>(
			identifier,
			std::move(fdecls)
		);
	}

	// field_declaration = identifier ":" type [ "=" expression ]  ;

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

		type_annotation = parse_type(); // parse_type handles erroe
		if (!type_annotation) {
			return nullptr;
		}

		std::unique_ptr<Expr> initializer = nullptr;
		if (match(TokenType::Equal)) {
			initializer = expression();
			if (!initializer) {
				error("expected expression after '='");
				return nullptr;
			}
		}

		return std::make_unique<FieldDecl>(
			identifier,
			std::move(type_annotation),
			std::move(initializer)
		);	
	}


	// class_declaration = "class" identifier [":" identifier] "{" { class_body } "}" ;
	// class_body = { class_member };
	// class_member = field_declaration | function_declaration;


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
			error("unable to parse class body");
			return nullptr;
		}

		return std::make_unique<ClassDecl>(
			class_name,
			parent_class,
			std::move(body)
		);
	}

	std::vector<ClassMember> Parser::class_body() {
		std::vector<ClassMember> members;

		auto left_brace = consume(TokenType::LeftBrace);
		if (!left_brace.has_value()) {
			error("expected '{' before class body");
			return {}; // Return empty vector on error
		}

		while (!check(TokenType::RightBrace) && !is_at_end()) {
			// Look ahead to determine if it's a field or function
			if (check(TokenType::Identifier) && check_next(TokenType::Colon)) { // field declaration
				auto field = field_declaration();
				if (field) {
					members.push_back(ClassMember::make_field(
						std::unique_ptr<FieldDecl>(static_cast<FieldDecl*>(field.release()))
					));
				}
				else {
					// Error already reported by field_declaration()
					return {};
				}
			}
			else if (check(TokenType::Fn) || (check(TokenType::Pub) && check_next(TokenType::Fn)) || 
				(check(TokenType::Modifies) && check_next(TokenType::Fn))) { // pub fn, modifies fn and pub modifies fn and fn (WE ARE HERE
				auto fn = fn_declaration();
				if (fn) {
					members.push_back(ClassMember::make_function(
						std::unique_ptr<FnDecl>(static_cast<FnDecl*>(fn.release()))
					));
				}
				else {
					// Error already reported by fn_declaration()
					return {};
				}
			}
			else {
				error("expected field or function declaration in class body");
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

	// trait_declaration = "trait" identifier "{" { function_declaration } "}" ;
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
				return nullptr; // error already reported by fn_declaration()
			}
		}

		auto right_brace = consume(TokenType::RightBrace);
		if (!right_brace.has_value()) {
			error("expected '}' after trait body");
			return nullptr;
		}

		return std::make_unique<TraitDecl>(
			trait_name,
			std::move(fndecls)
		);

	}
	// with_declaration = "with" identifier "for" identifier "{" { function_declaration } "}" ;


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
				return nullptr; // error already reported by fn_declaration()
			}
		}

		auto right_brace = consume(TokenType::RightBrace);
		if (!right_brace.has_value()) {
			error("expected '}' after trait body");
			return nullptr;
		}

		return std::make_unique<WithDecl>(
			trait_name,
			type_name,
			std::move(fndecls)
		);
	}

	// Expressions

	std::unique_ptr<Expr>  Parser::expression() {
		return assignment();
	}

	std::unique_ptr<Expr> Parser::assignment() {
		auto expr = conditional();

		if (match_assignment_op()) { 
			if (!expr->is_lvalue()) {
				error("cannot assign to this expression - not a valid assignment target");
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
			auto token = consume(TokenType::Else);

			if (token.has_value()) {
				auto else_expr = conditional();

				return std::make_unique<Conditional>(std::move(expr),
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

		return cast();
	}
	std::unique_ptr<Expr> Parser::cast() {
		auto expr = postfix();

		if (match(TokenType::As)) {  
			auto type = parse_type();
			if (!type) {
				return nullptr;
			}
			return std::make_unique<Cast>(std::move(expr), type);
		}

		return expr;
	}

	std::unique_ptr<Expr> Parser::postfix() {
		auto expr = primary();

		while (true) {
			// Array access
			if (match(TokenType::LeftBracket)) {
				auto index = expression();
				auto token = consume(TokenType::RightBracket);
				if (token.has_value()) {
					expr = std::make_unique<ArrayAccess>(std::move(expr), std::move(index));
				}
				else {
					error("expected ']' after array index");
				}
			}
			// Field Access
			else if (match(TokenType::Period)) {
				auto name = consume(TokenType::Identifier);
				if (name.has_value()) {
					expr = std::make_unique<FieldAccess>(std::move(expr), name);
				}
				else {
					error("expected property name after '.'");
					// an error has occurred
				}
			}
			// Arguments
			else if (match(TokenType::LeftParen)) {
				std::vector<std::unique_ptr<Expr>> args;
				if (!check(TokenType::RightParen)) {
					args = argument_list();
				}
				
				auto token = consume(TokenType::RightParen);
				if (token.has_value()) {
					expr = std::make_unique<FnCall>(std::move(expr), std::move(args));
				}
				else {
					error("expected ')' after arguments");
				}
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
		// Literals
		if (match(TokenType::IntLiteral, TokenType::FloatLiteral,
			TokenType::CharacterLiteral, TokenType::StringLiteral)) {
			return std::make_unique<Literal>(prev().value);
		}
		if (match(TokenType::True)) {
			return std::make_unique<Literal>(true);
		}
		if (match(TokenType::False)) {
			return std::make_unique<Literal>(false);
		}

		// Identifier
		if (match(TokenType::Identifier)) {
			return std::make_unique<Identifier>(prev());
		}

		// Grouping
		if (match(TokenType::LeftParen)) {
			auto expr = expression();
			auto token = consume(TokenType::RightParen);
			if (token.has_value()) {
				return std::make_unique<Grouping>(std::move(expr));
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
		args.push_back(expression());

		while (match(TokenType::Comma)) {
			args.push_back(expression());
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
		if (match(TokenType::LeftBrace)) {
			return block();
		}

		return expression_statement();
	}

	std::unique_ptr<Stmt> Parser::expression_statement() {
		auto expr = expression();
		auto token = consume(TokenType::Semicolon); // TODO: Add newline handling

		if (token.has_value()) {
			return std::make_unique<Stmt>(std::move(expr));
		}
		else {
			error("expected ';' after expression");
			return nullptr;
		}
	}

	std::unique_ptr<Stmt> Parser::return_statement() {
		Token keyword = prev();
		std::unique_ptr<Expr> expr;

		if (!check(TokenType::Semicolon)) { // or new line
			expr = expression();
		}

		auto token = consume(TokenType::Semicolon);
		if (token.has_value()) {
			return std::make_unique<ReturnStmt>(keyword, std::move(expr));
		}
		else {
			error("expected ';' after return value", "a return statement must end with a ';'");
			return nullptr;
		}
	}

	std::unique_ptr<Stmt> Parser::if_statement() {
		std::unique_ptr<Expr> condition = expression();
		std::unique_ptr<Stmt> then_branch = statement();
		std::unique_ptr<Stmt> else_branch;

		if (match(TokenType::Else)) {
			else_branch = statement();
		}

		return std::make_unique<IfStmt>(std::move(condition), std::move(then_branch), std::move(else_branch));
	}

	std::unique_ptr<Stmt> Parser::while_statement() {
		std::unique_ptr<Expr> condition = expression();
		std::unique_ptr<Stmt> body = statement();
		return std::make_unique<WhileStmt>(std::move(condition), std::move(body));
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

		if (match(TokenType::PeriodPeriod, TokenType::PeriodPeriodEqual)) {
			// Range for loop
			auto end_expr = expression();
			auto body = statement();
			return std::make_unique<RangeForStmt>(identifier, start_expr, end_expr, body);
		}
		else {
			// Iterator for loop  
			auto body = statement();
			return std::make_unique<IteratorForStmt>(identifier, start_expr, body);
		}
	}

	std::unique_ptr<Stmt> Parser::loop_statement() {
		std::unique_ptr<Stmt> body = statement();
		return std::make_unique<LoopStmt>(std::move(body));
	}

	std::unique_ptr<BlockStmt> Parser::block() {
		std::vector<std::unique_ptr<Stmt>> stmts;

		auto left_brace = consume(TokenType::LeftBrace);
		if (!left_brace.has_value()) {
			error("expected '{' before block", "a block must always have an opening '{'");
			return nullptr;
		}

		while (!check(TokenType::RightBrace) && !is_at_end()) {
			stmts.push_back(std::move(statement()));
		}

		auto token = consume(TokenType::RightBrace);
		if (token.has_value()) {
			return std::make_unique<BlockStmt>(std::move(stmts));
		}
		else {
			error("expected '}' after block", "a block must always have a closing '}' for every open '{'");
			return nullptr;
		}
	}

	std::unique_ptr<Type> Parser::parse_type() {
		std::unique_ptr<Type> base_type;

		auto identifier = consume(TokenType::Identifier);
		if (!identifier.has_value()) {
			error("expected type name");
			return nullptr;
		}

		if (is_primitive_type()) {
			base_type = parse_primitive_type(identifier.value().lexeme);
		}
		else {
			if (match(TokenType::Exclaim)) {
				if (match(TokenType::Less)) {
					base_type = parse_generic_type(identifier.value().lexeme);
				}
				else {
					error("expected '<' for generic type");
					return nullptr;
				}
			}
			else {
				base_type = parse_user_type(identifier.value().lexeme);
			}
		}

		if (check(TokenType::LeftBracket)) { // array dimensions
			auto dimensions = parse_array_dimensions();
			if (dimensions.empty()) { // parse_array_dimensions() already called error()
				return nullptr;
			}
			return std::make_unique<ArrayType>(
				std::move(base_type),
				std::move(dimensions)
			);
		}

		return base_type;
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
	
		return prim_type; // if NOT a primitive type, still could be a generic / user type, which is why we don't report an error
	}

	std::unique_ptr<Type> Parser::parse_user_type(const std::string& name) {
		auto user_type = std::make_unique<UserType>();
		user_type->name = name;
		return user_type;
	}

	std::unique_ptr<Type> Parser::parse_generic_type(const std::string& name) {
		std::vector<std::unique_ptr<Type>> type_arguments;

		type_arguments.push_back(parse_type());
		while (match(TokenType::Comma)) {
			type_arguments.push_back(parse_type());
		}

		auto greater = consume(TokenType::Greater);

		if (!greater.has_value()) {
			error("expected '>' to close generic type");
			return nullptr;
		}

		return std::make_unique<GenericType>(
			name,
			std::move(type_arguments)
		);
	}

	std::vector<int> Parser::parse_array_dimensions() {
		std::vector<int32_t> dimensions;
		bool has_unsized = false;

		while (peek().type == TokenType::LeftBracket) {
			advance(); // consume '['

			if (peek().type == TokenType::IntLiteral) { // sized dimension
				if (has_unsized) {
					error("cannot have sized dimension after unsized dimension");
					return {}; // return empty vector to indicate error
				}
				dimensions.push_back(std::get<int>(peek().value));
				advance();
			}
			else if (peek().type == TokenType::RightBracket) { // unsized dimension
				dimensions.push_back(-1);
				has_unsized = true;
			}
			else { // invalid content inside brackets
				error("expected integer literal or ']' in array dimension");
				return {};
			}

			auto right_bracket = consume(TokenType::RightBracket);
			if (!right_bracket.has_value()) {
				error("expected ']' to close array dimension");
				return {};
			}
		}

		return dimensions;
	}

	bool Parser::is_primitive_type() {
		switch (peek().type) {
			case TokenType::IntLiteral:
			case TokenType::FloatLiteral:
			case TokenType::StringLiteral:
			case TokenType::CharacterLiteral:
			case TokenType::True:
			case TokenType::False:
			return true;

		default:
			return false;
		}
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

	bool Parser::check_fn_declaration() { //
		return check(TokenType::Fn) ||
			(check(TokenType::Pub) && check_next(TokenType::Fn)) ||
			(check(TokenType::Modifies) && check_next(TokenType::Fn));
	}

	void Parser::error(const std::string& msg, const std::string& note) {
		Token curr = peek();
		reporter_.add_error(source_context_.filename(), curr.lexeme.length(), curr.loc, msg, source_context_.get_line(curr.loc.line), note);
	}
}