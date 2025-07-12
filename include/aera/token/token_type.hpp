#pragma once 

#include <unordered_map>
#include <unordered_set>
#include <string>
#include <cstdint>

namespace aera {

	enum class TokenType {

        // Punctuation

		LeftParen,                  // (
        RightParen,	                // )
		LeftBrace,                  // {
        RightBrace,                 // }
		LeftBracket,                // [ 
        RightBracket,               // ]
		Comma,                      // ,
        Period,                     // .
		Semicolon,                  // ;
        Colon,                      // ;
		
        // Operators (from longest to shortest)

        LessLessEqual,              // <<=
        GreaterGreaterEqual,        // >>=
        PeriodPeriodEqual,          // ..=

        AmpAmp,                     // &&
        PipePipe,                   // ||
        EqualEqual,		            // ==
        ExclaimEqual,	            // !=
        GreaterEqual,               // >=
        LessEqual,		            // <=
        GreaterGreater,             // >>
        LessLess,                   // <<
        PlusPlus,                   // ++
        MinusMinus,                 // --
        PlusEqual,                  // +=
        MinusEqual,                 // -=
        StarEqual,                  // *=
        SlashEqual,                 // /=
        PercentEqual,               // %=
        AmpEqual,                   // &=
        PipeEqual,                  // |=
        CaretEqual,                 // ^=
        TildeEqual,                 // ~=
        MinusGreater,               // ->
        PeriodPeriod,               // ..
        EqualGreater,               // =>

        Amp,                        // &
        Pipe,                       // |
        Caret,                      // ^
        Tilde,                      // ~
        Plus,                       // +
        Minus,                      // -
        Star,                       // *
        Slash,                      // /
        Percent,                    // %
        Question,                   // ?
        At,                         // @
        Exclaim,                    // !
        Equal,                      // =
        Greater,                    // >
        Less,                       // <
        
        // Comments

        LineComment,                // #
        BlockComment,               // <# ... #>

		// Literals

		Identifier, IntLiteral, FloatLiteral, CharacterLiteral, StringLiteral, BoolLiteral,

		// Keywords

		Fn, Let, Mut, Const, Pub,
		If, Else, For, While, Loop, Match, Break, Continue, Return,
		Import, Class, Struct, Enum, Interface, Alias, Self, As,
		In, True, False, None, Bind,

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
        {"none", TokenType::None},
        {"cpp", TokenType::Bind}
    };

    inline const std::unordered_set<std::string> valid_int_suffixes = {
    "i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64"
    };
    
    inline const std::unordered_set<std::string> valid_float_suffixes = {
        "f32", "f64"
    };
}