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

        // New line
        Newline,
		
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

		Identifier, IntLiteral, FloatLiteral, CharacterLiteral, StringLiteral, True, False,

		// Keywords

		Fn, Let, Mut, Const, Pub,
		If, Else, For, While, Loop, Match, Break, Continue, Return, In, 
		Import, Class, Struct, Enum, Trait, Modifies, Alias, Self, As, Bind,
        True, False, None,

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
        {"trait", TokenType::Trait},
        {"modifies", TokenType::Modifies},
        {"alias", TokenType::Alias},
        {"self", TokenType::Self},
        {"as", TokenType::As},
        {"in", TokenType::In},
        {"true", TokenType::True},
        {"false", TokenType::False},
        {"none", TokenType::None},
        {"bind", TokenType::Bind}
    };

    inline const std::unordered_set<std::string> valid_int_suffixes = {
    "i8", "u8", "i16", "u16", "i32", "u32", "i64", "u64"
    };
    
    inline const std::unordered_set<std::string> valid_float_suffixes = {
        "f32", "f64"
    };
}