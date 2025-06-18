#pragma once 

namespace aera::lexer {

	enum class TokenType {

		// Single-character tokens
		LeftParen, RightParen,		 // ()
		LeftBrace, RightBrace,		 // {}
		LeftBracket, RightBracket,	 // []
		Comma, Dot, Minus, Plus,	 // , . - +
		Semicolon, Colon, Slash,     // ; : /
		Star, Question,				 // * ?

		// One or two character tokens
		Exclaim, ExclaimEqual,	 // !, !=
		Equal, DoubleEqual,		 // =, ==
		Greater, GreaterEqual,	 // >, >=
		Less, LessEqual,		 // <, <=
		Comment,			     // #
		MultiComment,			 // <# ... #>

		// Literals
		Identifier, Int, Float, Char, String, Bool,

		// Keywords
		Fn, Let, Mut, Const, Pub,
		If, Else, For, While, Loop, Math, Break, Continue, Return,
		Import, Class, Struct, Enum, Abstract, Interface, Alias, Self, As,
		True, False, Null, None, Print
	};
}