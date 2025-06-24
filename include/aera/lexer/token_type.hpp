#pragma once 

#include <unordered_map>
#include <string>

namespace aera::lexer {

	enum class TokenType {

        // Punctuation

		LeftParen, RightParen,		 // ()
		LeftBrace, RightBrace,		 // {}
		LeftBracket, RightBracket,	 // []
		Comma, Period,	             // , .
		Semicolon, Colon,            // ; :
		
        // Potential operators

        Question,                    // ? (used for potential nullable types)
        At,                          // @ (potential uses: macros)

        // Arithmetic operators

        PlusOp, MinusOp, StarOp, SlashOp, PercentOp, // + - * / %

        // Comparison operators

        ExclaimOp, ExclaimEqualOp,	 // !, !=
        AssignOp, EqualOp,		     // =, ==
        GreaterOp, GreaterEqualOp,	 // >, >=
        LessOp, LessEqualOp,		 // <, <=

        // Logical operators

        LogicalAndOp,                // &&
        LogicalOrOp,                 // ||

        // Bitwise operators (added because easy to parse)

        BitwiseAndOp,                // &
        BitwiseOrOp,                 // |

        // Other operators

        ArrowOp,                     // Used in functions to declare return type

        // Comments

        LineComment,                 // #
        BlockComment,                // <# ... #>

		// Literals

		Identifier, IntLiteral, FloatLiteral, CharacterLiteral, StringLiteral, BoolLiteral,

		// Keywords

		Fn, Let, Mut, Const, Pub,
		If, Else, For, While, Loop, Match, Break, Continue, Return,
		Import, Class, Struct, Enum, Abstract, Interface, Alias, Self, As,
		True, False, Null, None, Print, Unsafe,

		Eof
	};

    inline const std::unordered_map<std::string, TokenType> keywords = {
        {"fn", TokenType::Fn},
        {"let", TokenType::Let},
        {"mut", TokenType::Mut},
        {"const", TokenType::Const},
        {"pub", TokenType::Pub},
        {"if", TokenType::If},
        {"else", TokenType::Else},
        {"for", TokenType::For},
        {"while", TokenType::While},
        {"loop", TokenType::Loop},
        {"match", TokenType::Match},
        {"break", TokenType::Break},
        {"continue", TokenType::Continue},
        {"return", TokenType::Return},
        {"import", TokenType::Import},
        {"class", TokenType::Class},
        {"struct", TokenType::Struct},
        {"enum", TokenType::Enum},
        {"abstract", TokenType::Abstract},
        {"interface", TokenType::Interface},
        {"alias", TokenType::Alias},
        {"self", TokenType::Self},
        {"as", TokenType::As},
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"null", TokenType::Null},
        {"none", TokenType::None},
        {"print", TokenType::Print},
        {"unsafe", TokenType::Unsafe}
    };
}