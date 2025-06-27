#pragma once 

#include <unordered_map>
#include <unordered_set>
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
        RangeInclusiveOp,            // 0..=10, including 10 -> used in for loops
        RangeExclusiveOp,            // 0..10, not including 10 -> used in for loops
        // Comments

        LineComment,                 // #
        BlockComment,                // <# ... #>

		// Literals

		Identifier, IntLiteral, FloatLiteral, CharacterLiteral, StringLiteral, BoolLiteral,

		// Keywords

		Fn, Let, Mut, Const, Pub,
		If, Else, For, While, Loop, Match, Break, Continue, Return,
		Import, Class, Struct, Enum, Interface, Alias, Self, As,
		In, True, False, Null, None, Cpp,

		Illegal, Eof
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
        {"interface", TokenType::Interface},
        {"alias", TokenType::Alias},
        {"self", TokenType::Self},
        {"as", TokenType::As},
        {"in", TokenType::In},
        {"true", TokenType::BoolLiteral},
        {"false", TokenType::BoolLiteral},
        {"null", TokenType::Null},
        {"none", TokenType::None},
        {"cpp", TokenType::Cpp}
    };

    inline const std::unordered_set<std::string> valid_int_suffixes = {
    "i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64"
    };
    
    inline const std::unordered_set<std::string> valid_float_suffixes = {
        "f32", "f64"
    };
}