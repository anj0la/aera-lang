#include <gtest/gtest.h>
#include <aera/lexer/lexer.hpp>
#include <iostream>

using namespace aera::lexer;

// Basic Token Recognition

TEST(LexerTest, TokenizesLetStatement) {
	// Arrange
	std::string input = "let x = 5;";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,		"let", 1, 1},
		{TokenType::Identifier, "x",   1, 5},
		{TokenType::AssignOp,	"=",   1, 7},
		{TokenType::IntLiteral, "5",   1, 9},
		{TokenType::Semicolon,	";",   1, 10},
		{TokenType::Eof,		"",    1, 11}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesIfStatement) {
	// Arrange
	std::string input = "if true { do_something(); }";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::If,			 "if",			 1, 1},
		{TokenType::BoolLiteral, "true",		 1, 4},
		{TokenType::LeftBrace,   "{",			 1, 9},
		{TokenType::Identifier,	 "do_something", 1, 11},
		{TokenType::LeftParen,   "(",			 1, 23},
		{TokenType::RightParen,  ")",			 1, 24},
		{TokenType::Semicolon,   ";",			 1, 25},
		{TokenType::RightBrace,  "}",			 1, 27},
		{TokenType::Eof,		 "",			 1, 28}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesWhileStatement) {
	// Arrange
	std::string input = "while x > 10 { do_something(); }";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::While,         "while",        1, 1},
		{TokenType::Identifier,    "x",            1, 7},
		{TokenType::GreaterOp,     ">",            1, 9},
		{TokenType::IntLiteral,    "10",           1, 11},
		{TokenType::LeftBrace,     "{",            1, 14},
		{TokenType::Identifier,    "do_something", 1, 16},
		{TokenType::LeftParen,     "(",            1, 28},
		{TokenType::RightParen,    ")",            1, 29},
		{TokenType::Semicolon,     ";",            1, 30},
		{TokenType::RightBrace,    "}",            1, 32},
		{TokenType::Eof,           "",             1, 33} 
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesForStatement_Exclusive) {
	// Arrange
	std::string input = "for i in 0..10 { do_something(); }";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::For,				"for",          1, 1},
		{TokenType::Identifier,			"i",            1, 5},
		{TokenType::In,					"in",           1, 7},
		{TokenType::IntLiteral,			"0",            1, 10},
		{TokenType::RangeExclusiveOp,	"..",           1, 11},
		{TokenType::IntLiteral,			"10",           1, 13},
		{TokenType::LeftBrace,			"{",            1, 16},
		{TokenType::Identifier,			"do_something", 1, 18},
		{TokenType::LeftParen,			"(",            1, 30},
		{TokenType::RightParen,			")",            1, 31},
		{TokenType::Semicolon,			";",            1, 32},
		{TokenType::RightBrace,			"}",            1, 34},
		{TokenType::Eof,				"",             1, 35}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesForStatement_Exclusive_WithIntTypes) {
	// Arrange
	std::string input = "for i in 0i32..10i32 { do_something(); }";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::For,				"for",          1, 1},
		{TokenType::Identifier,			"i",            1, 5},
		{TokenType::In,					"in",           1, 7},
		{TokenType::IntLiteral,			"0i32",         1, 10},
		{TokenType::RangeExclusiveOp,	"..",           1, 14},
		{TokenType::IntLiteral,			"10i32",        1, 16},
		{TokenType::LeftBrace,			"{",            1, 22},
		{TokenType::Identifier,			"do_something", 1, 24},
		{TokenType::LeftParen,			"(",            1, 36},
		{TokenType::RightParen,			")",            1, 37},
		{TokenType::Semicolon,			";",            1, 38},
		{TokenType::RightBrace,			"}",            1, 40},
		{TokenType::Eof,				"",             1, 41}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesForStatement_Inclusive) {
	// Arrange
	std::string input = "for i in 0..=10 { do_something(); }";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::For,				"for",          1, 1},
		{TokenType::Identifier,			"i",            1, 5},
		{TokenType::In,					"in",           1, 7},
		{TokenType::IntLiteral,			"0",            1, 10},
		{TokenType::RangeInclusiveOp,	"..=",          1, 11},
		{TokenType::IntLiteral,			"10",           1, 14},
		{TokenType::LeftBrace,			"{",            1, 17},
		{TokenType::Identifier,			"do_something", 1, 19},
		{TokenType::LeftParen,			"(",            1, 31},
		{TokenType::RightParen,			")",            1, 32},
		{TokenType::Semicolon,			";",            1, 33},
		{TokenType::RightBrace,			"}",            1, 35},
		{TokenType::Eof,				"",             1, 36}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesLoopStatement) {
	// Arrange
	std::string input = "loop { do_something(); }";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::Loop,          "loop",         1, 1},
		{TokenType::LeftBrace,     "{",            1, 6},
		{TokenType::Identifier,    "do_something", 1, 8},
		{TokenType::LeftParen,     "(",            1, 20},
		{TokenType::RightParen,    ")",            1, 21},
		{TokenType::Semicolon,     ";",            1, 22},
		{TokenType::RightBrace,    "}",            1, 24},
		{TokenType::Eof,           "",             1, 25}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesFunctionDeclaration) {
	// Arrange
	std::string input = "pub fn public_add(a: int32, b: int32) -> int32 { return a + b; }";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::Pub,         "pub",        1, 1},
		{TokenType::Fn,          "fn",         1, 5},
		{TokenType::Identifier,  "public_add", 1, 8},
		{TokenType::LeftParen,   "(",          1, 18},
		{TokenType::Identifier,  "a",          1, 19},
		{TokenType::Colon,       ":",          1, 20},
		{TokenType::Identifier,  "int32",      1, 22},
		{TokenType::Comma,       ",",          1, 27},
		{TokenType::Identifier,  "b",          1, 29},
		{TokenType::Colon,       ":",          1, 30},
		{TokenType::Identifier,  "int32",      1, 32},
		{TokenType::RightParen,  ")",          1, 37},
		{TokenType::ArrowOp,     "->",         1, 39},
		{TokenType::Identifier,  "int32",      1, 42},
		{TokenType::LeftBrace,   "{",          1, 48},
		{TokenType::Return,      "return",     1, 50},
		{TokenType::Identifier,  "a",          1, 57},
		{TokenType::PlusOp,      "+",          1, 59},
		{TokenType::Identifier,  "b",          1, 61},
		{TokenType::Semicolon,   ";",          1, 62},
		{TokenType::RightBrace,  "}",          1, 64},
		{TokenType::Eof,         "",           1, 65}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesMultiSentence) {
	// Arrange
	std::string input = R"(let val = 10;
# A comment to skip
  fn_call();
)";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::Let,         "let",     1, 1},
		{TokenType::Identifier,  "val",     1, 5},
		{TokenType::AssignOp,    "=",       1, 9},
		{TokenType::IntLiteral,  "10",      1, 11},
		{TokenType::Semicolon,   ";",       1, 13},
		{TokenType::Identifier,  "fn_call", 3, 3},
		{TokenType::LeftParen,   "(",       3, 10},
		{TokenType::RightParen,  ")",       3, 11},
		{TokenType::Semicolon,   ";",       3, 12},
		{TokenType::Eof,         "",        4, 1}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

// Edge Cases and Boundary Conditions

TEST(LexerTest, EmptyInput_ProducesOnlyEof) {
	// Arrange
	std::string input = "";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Eof, "", 1, 1}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	EXPECT_EQ(expected_tokens[0], actual_tokens[0]);
}

TEST(LexerTest, WhitespaceOnlyInput_ProducesOnlyEof) {
	// Arrange
	std::string input = " \t \r\n   ";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Eof, "", 2, 4}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	EXPECT_EQ(expected_tokens[0], actual_tokens[0]);
}

// String and Character Literals

TEST(LexerTest, TokenizesStringWithNewlineEscape) {
	// Arrange
	std::string input = "let s = \"hello\\nworld\";";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,            "let",           1, 1},
		{TokenType::Identifier,     "s",             1, 5},
		{TokenType::AssignOp,       "=",             1, 7},
		{TokenType::StringLiteral,  "hello\nworld",  1, 9},
		{TokenType::Semicolon,      ";",             1, 23},
		{TokenType::Eof,            "",              1, 24}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesEmptyString) {
	// Arrange
	std::string input = "let empty = \"\";";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,            "let",     1, 1},
		{TokenType::Identifier,     "empty",   1, 5},
		{TokenType::AssignOp,       "=",       1, 11},
		{TokenType::StringLiteral,  "",        1, 13},
		{TokenType::Semicolon,      ";",       1, 15},
		{TokenType::Eof,            "",        1, 16}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, HandlesEscapedQuotesAndBackslashes) { 
	// Arrange
	std::string input = "let s = \"a \\\"quoted\\\" string and a \\\\ backslash\";";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,            "let",                        1, 1},
		{TokenType::Identifier,     "s",                          1, 5},
		{TokenType::AssignOp,       "=",                          1, 7},
		{TokenType::StringLiteral,  "a \"quoted\" string and a \\ backslash", 1, 9},
		{TokenType::Semicolon,      ";",                          1, 49},
		{TokenType::Eof,            "",                           1, 50}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, HandlesUnterminatedString) {
	// Arrange
	std::string input = "let s = \"abc";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,        "let",                         1, 1},
		{TokenType::Identifier, "s",                           1, 5},
		{TokenType::AssignOp,   "=",                           1, 7},
		{TokenType::Illegal,    "Unterminated string literal.", 1, 9},
		{TokenType::Eof,        "",                            1, 13}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
	EXPECT_TRUE(lexer.has_error());
}

TEST(LexerTest, HandlesInvalidEscapeInString) {
	// Arrange
	std::string input = "\"hello \\z world\"";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Illegal,    "Invalid escape sequence: \\z", 1, 1},
		{TokenType::Eof,        "",                             1, 17}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
	EXPECT_TRUE(lexer.has_error());
}

// Character

TEST(LexerTest, TokenizesSimpleCharacter) {
	// Arrange
	std::string input = "let c = 'a';";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,             "let", 1, 1},
		{TokenType::Identifier,      "c",   1, 5},
		{TokenType::AssignOp,        "=",   1, 7},
		{TokenType::CharacterLiteral,"a",   1, 9},
		{TokenType::Semicolon,       ";",   1, 12},
		{TokenType::Eof,             "",    1, 13}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesEscapedCharacter) {
	// Arrange
	std::string input = "let newline = '\\n';";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,             "let",     1, 1},
		{TokenType::Identifier,      "newline", 1, 5},
		{TokenType::AssignOp,        "=",       1, 13},
		{TokenType::CharacterLiteral,"\n",      1, 15},
		{TokenType::Semicolon,       ";",       1, 19},
		{TokenType::Eof,             "",        1, 20}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, HandlesEmptyCharacterLiteral) {
	// Arrange
	std::string input = "let c = '';";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,        "let",                       1, 1},
		{TokenType::Identifier, "c",                         1, 5},
		{TokenType::AssignOp,   "=",                         1, 7},
		{TokenType::Illegal,    "Empty character literal.",  1, 9},
		{TokenType::Semicolon,  ";",                         1, 11},
		{TokenType::Eof,        "",                          1, 12}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
	EXPECT_TRUE(lexer.has_error());
}

// REVISED ERROR TEST
TEST(LexerTest, HandlesTooLongCharacterLiteral) {
	// Arrange
	std::string input = "let c = 'ab';";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,        "let",													1, 1},
		{TokenType::Identifier, "c",													1, 5},
		{TokenType::AssignOp,   "=",													1, 7},
		{TokenType::Illegal,    "Character literal must contain only one character.",	1, 9},
		{TokenType::Semicolon,  ";",													1, 13},
		{TokenType::Eof,        "",														1, 14}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
	EXPECT_TRUE(lexer.has_error());
}

// Numeric Literals

TEST(LexerTest, TokenizesIntegerLiterals) {
	// Arrange
	std::string input = "let answer = 42;";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,         "let",    1, 1},
		{TokenType::Identifier,  "answer", 1, 5},
		{TokenType::AssignOp,    "=",      1, 12},
		{TokenType::IntLiteral,  "42",     1, 14},
		{TokenType::Semicolon,   ";",      1, 16},
		{TokenType::Eof,         "",       1, 17}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesFloatLiterals) {
	// Arrange
	std::string input = "let pi = 3.14;";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,			"let",    1, 1},
		{TokenType::Identifier,		"pi",     1, 5},
		{TokenType::AssignOp,		"=",      1, 8},
		{TokenType::FloatLiteral,	"3.14",  1, 10},
		{TokenType::Semicolon,		";",      1, 14},
		{TokenType::Eof,			"",       1, 15}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesNumbersWithSuffixes) {
	// Arrange
	std::string input = "let num1 = 42i32; let num2 = 5u32; let num3 = 3.14f32;";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,            "let",     1, 1},
		{TokenType::Identifier,     "num1",    1, 5},
		{TokenType::AssignOp,       "=",       1, 10},
		{TokenType::IntLiteral,		"42i32",   1, 12},
		{TokenType::Semicolon,      ";",       1, 17},
		{TokenType::Let,            "let",     1, 19},
		{TokenType::Identifier,     "num2",    1, 23},
		{TokenType::AssignOp,       "=",       1, 28},
		{TokenType::IntLiteral,		"5u32",    1, 30},
		{TokenType::Semicolon,      ";",       1, 34},
		{TokenType::Let,            "let",     1, 36},
		{TokenType::Identifier,     "num3",    1, 40},
		{TokenType::AssignOp,       "=",       1, 45},
		{TokenType::FloatLiteral,   "3.14f32", 1, 47},
		{TokenType::Semicolon,      ";",       1, 54},
		{TokenType::Eof,            "",        1, 55}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesIntegerWithFloatSuffixAsFloat) {
	// Arrange
	std::string input = "let x = 3f32;";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,           "let",   1, 1},
		{TokenType::Identifier,    "x",     1, 5},
		{TokenType::AssignOp,      "=",     1, 7},
		{TokenType::FloatLiteral,  "3f32",  1, 9},
		{TokenType::Semicolon,     ";",     1, 13},
		{TokenType::Eof,           "",      1, 14}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesFloatWithTrailingDot) {
	// Arrange
	std::string input = "let pi = 3.;";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,           "let",  1, 1},
		{TokenType::Identifier,    "pi",   1, 5},
		{TokenType::AssignOp,      "=",    1, 8},
		{TokenType::FloatLiteral,  "3.",   1, 10},
		{TokenType::Semicolon,     ";",    1, 12},
		{TokenType::Eof,           "",     1, 13}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, HandlesMalformedNumberWithExtraDot) {
	// Arrange
	std::string input = "let val = 3.14.;"; // An extra dot at the end
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,        "let",                             1, 1},
		{TokenType::Identifier, "val",                             1, 5},
		{TokenType::AssignOp,   "=",                               1, 9},
		{TokenType::Illegal,    "Malformed number literal: 3.14.", 1, 11},
		{TokenType::Semicolon,  ";",                               1, 16},
		{TokenType::Eof,        "",                                1, 17}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
	EXPECT_TRUE(lexer.has_error());
}

TEST(LexerTest, HandlesInvalidIntegerSuffix) {
	// Arrange
	std::string input = "let x = 123i99;";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,         "let",                     1, 1},
		{TokenType::Identifier,  "x",                       1, 5},
		{TokenType::AssignOp,    "=",                       1, 7},
		{TokenType::Illegal,     "Invalid integer suffix: 123i99", 1, 9},
		{TokenType::Semicolon,   ";",                       1, 15},
		{TokenType::Eof,         "",                        1, 16}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
	EXPECT_TRUE(lexer.has_error());
}

TEST(LexerTest, HandlesInvalidFloat_InForStatement) {
	// Arrange
	std::string input = "for i in 1.0..10.0 { do_something(); }";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::For,           "for",													1, 1},
		{TokenType::Identifier,    "i",														1, 5},
		{TokenType::In,            "in",													1, 7},
		{TokenType::Illegal,       "Range operator cannot follow a float literal: 1.0..",	1, 10},
		{TokenType::FloatLiteral,  "10.0",													1, 15},
		{TokenType::LeftBrace,     "{",														1, 20},
		{TokenType::Identifier,    "do_something",											1, 22},
		{TokenType::LeftParen,     "(",														1, 34},
		{TokenType::RightParen,    ")",														1, 35},
		{TokenType::Semicolon,     ";",														1, 36},
		{TokenType::RightBrace,    "}",														1, 38},
		{TokenType::Eof,           "",														1, 39}	
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
	EXPECT_TRUE(lexer.has_error());
}

TEST(LexerTest, HandlesInvalidFloatSuffix_InForStatement) {
	// Arrange
	std::string input = "for i in 0f32..10 { do_something(); }";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::For,           "for",														1, 1},
		{TokenType::Identifier,    "i",															1, 5},
		{TokenType::In,            "in",														1, 7},
		{TokenType::Illegal,       "Range operator cannot follow a float literal: 0f32..",		1, 10},
		{TokenType::IntLiteral,    "10",														1, 16},
		{TokenType::LeftBrace,     "{",															1, 19},
		{TokenType::Identifier,    "do_something",												1, 21},
		{TokenType::LeftParen,     "(",															1, 33},
		{TokenType::RightParen,    ")",															1, 34},
		{TokenType::Semicolon,     ";",															1, 35},
		{TokenType::RightBrace,    "}",															1, 37},
		{TokenType::Eof,           "",															1, 38}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
	EXPECT_TRUE(lexer.has_error());
}

// Comments

TEST(LexerTest, LineCommentOnly_ProducesOnlyEof) {
	// Arrange
	std::string input = "# this is a comment and should be ignored";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Eof, "", 1, 42}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	EXPECT_EQ(expected_tokens[0], actual_tokens[0]);
}

TEST(LexerTest, BlockCommentOnly_ProducesOnlyEof) {
	// Arrange
	std::string input = "<# this is a block comment and should be ignored #>";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Eof, "", 1, 52}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	EXPECT_EQ(expected_tokens[0], actual_tokens[0]);
}

TEST(LexerTest, TokensAndComments_IgnoresComments) {
	// Arrange
	std::string input = "let x = 5; # assign 5 to x";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,		"let", 1, 1},
		{TokenType::Identifier, "x",   1, 5},
		{TokenType::AssignOp,	"=",   1, 7},
		{TokenType::IntLiteral, "5",   1, 9},
		{TokenType::Semicolon,	";",   1, 10},
		{TokenType::Eof,		"",    1, 27}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, HandlesUnterminatedBlockComment) {
	// Arrange
	std::string input = "let x = 5; <# this is not closed";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,        "let",                      1, 1},
		{TokenType::Identifier, "x",                        1, 5},
		{TokenType::AssignOp,   "=",                        1, 7},
		{TokenType::IntLiteral, "5",                        1, 9},
		{TokenType::Semicolon,  ";",                        1, 10},
		{TokenType::Illegal,    "Unterminated block comment.", 1, 12},
		{TokenType::Eof,        "",                         1, 33}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
	EXPECT_TRUE(lexer.has_error());
}

TEST(LexerTest, HandlesMalformedBlockCommentTerminator) {
	// Arrange
	std::string input = "<# this is not closed again #";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Illegal, "Unterminated block comment.", 1, 1},
		{TokenType::Eof,     "",                                    1, 30}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
	EXPECT_TRUE(lexer.has_error());
}

// Keywords vs Identifiers

TEST(LexerTest, DistinguishesKeywordsFromIdentifiers) {
	// Arrange
	std::string input = "if ifdef { let return_value = true; }";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::If,          "if",             1, 1},
		{TokenType::Identifier,  "ifdef",          1, 4},
		{TokenType::LeftBrace,      "{",              1, 10},
		{TokenType::Let,         "let",            1, 12},
		{TokenType::Identifier,  "return_value",   1, 16},
		{TokenType::AssignOp,    "=",              1, 29},
		{TokenType::BoolLiteral,  "true",           1, 31},
		{TokenType::Semicolon,   ";",              1, 35},
		{TokenType::RightBrace,      "}",              1, 37},
		{TokenType::Eof,         "",               1, 38}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

// Error Handling

TEST(LexerTest, HandlesInvalidCharacter) {
	// Arrange
	std::string input = "let x = $;";
	Lexer lexer(input);
	std::vector<Token> expected_tokens = {
		{TokenType::Let,        "let", 1, 1},
		{TokenType::Identifier, "x",   1, 5},
		{TokenType::AssignOp,   "=",   1, 7},
		{TokenType::Illegal,    "$",   1, 9},
		{TokenType::Semicolon,  ";",   1, 10},
		{TokenType::Eof,        "",    1, 11}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < expected_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

// Multi-Line Examples

TEST(LexerTest, TokenizesHelloWorldProgram) {
	// Arrange
	std::string input = R"(fn main() -> None {
    print("Hello, World!");
}
)";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		{TokenType::Fn,            "fn",              1, 1},
		{TokenType::Identifier,    "main",            1, 4},
		{TokenType::LeftParen,     "(",               1, 8},
		{TokenType::RightParen,    ")",               1, 9},
		{TokenType::ArrowOp,       "->",              1, 11},
		{TokenType::Identifier,    "None",            1, 14},
		{TokenType::LeftBrace,     "{",               1, 19},
		{TokenType::Identifier,    "print",           2, 5},
		{TokenType::LeftParen,     "(",               2, 10},
		{TokenType::StringLiteral, "Hello, World!",   2, 11},
		{TokenType::RightParen,    ")",               2, 26},
		{TokenType::Semicolon,     ";",               2, 27},
		{TokenType::RightBrace,    "}",               3, 1},
		{TokenType::Eof,           "",                4, 1}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}

TEST(LexerTest, TokenizesComplexFunctionAndLoop) {
	// Arrange
	std::string input = R"(fn main() -> None {
    let ans = sum_up_to(5i32);
    print(ans);
}

fn sum_up_to(num: int32) -> int32 {
    let mut sum: int32 = 0;
    for i in 0..num {
        sum = sum + i;
    }
    return sum;
}
)";
	Lexer lexer(input);

	std::vector<Token> expected_tokens = {
		// main function
		{TokenType::Fn,            "fn",          1, 1},
		{TokenType::Identifier,    "main",        1, 4},
		{TokenType::LeftParen,     "(",           1, 8},
		{TokenType::RightParen,    ")",           1, 9},
		{TokenType::ArrowOp,       "->",          1, 11},
		{TokenType::Identifier,    "None",        1, 14},
		{TokenType::LeftBrace,     "{",           1, 19},
		{TokenType::Let,           "let",         2, 5},
		{TokenType::Identifier,    "ans",         2, 9},
		{TokenType::AssignOp,      "=",           2, 13},
		{TokenType::Identifier,    "sum_up_to",   2, 15},
		{TokenType::LeftParen,     "(",           2, 24},
		{TokenType::IntLiteral,    "5i32",        2, 25},
		{TokenType::RightParen,    ")",           2, 29},
		{TokenType::Semicolon,     ";",           2, 30},
		{TokenType::Identifier,    "print",       3, 5},
		{TokenType::LeftParen,     "(",           3, 10},
		{TokenType::Identifier,    "ans",         3, 11},
		{TokenType::RightParen,    ")",           3, 14},
		{TokenType::Semicolon,     ";",           3, 15},
		{TokenType::RightBrace,    "}",           4, 1},

		// sum_up_to function
		{TokenType::Fn,               "fn",          6, 1},
		{TokenType::Identifier,       "sum_up_to",   6, 4},
		{TokenType::LeftParen,        "(",           6, 13},
		{TokenType::Identifier,       "num",         6, 14},
		{TokenType::Colon,            ":",           6, 17},
		{TokenType::Identifier,       "int32",       6, 19},
		{TokenType::RightParen,       ")",           6, 24},
		{TokenType::ArrowOp,          "->",          6, 26},
		{TokenType::Identifier,       "int32",       6, 29},
		{TokenType::LeftBrace,        "{",           6, 35},
		{TokenType::Let,              "let",         7, 5},
		{TokenType::Mut,              "mut",         7, 9},
		{TokenType::Identifier,       "sum",         7, 13},
		{TokenType::Colon,            ":",           7, 16},
		{TokenType::Identifier,       "int32",       7, 18},
		{TokenType::AssignOp,         "=",           7, 24},
		{TokenType::IntLiteral,       "0",           7, 26},
		{TokenType::Semicolon,        ";",           7, 27},
		{TokenType::For,              "for",         8, 5},
		{TokenType::Identifier,       "i",           8, 9},
		{TokenType::In,               "in",          8, 11},
		{TokenType::IntLiteral,       "0",           8, 14},
		{TokenType::RangeExclusiveOp, "..",          8, 15},
		{TokenType::Identifier,       "num",         8, 17},
		{TokenType::LeftBrace,        "{",           8, 21},
		{TokenType::Identifier,       "sum",         9, 9},
		{TokenType::AssignOp,         "=",           9, 13},
		{TokenType::Identifier,       "sum",         9, 15},
		{TokenType::PlusOp,           "+",           9, 19},
		{TokenType::Identifier,       "i",           9, 21},
		{TokenType::Semicolon,        ";",           9, 22},
		{TokenType::RightBrace,       "}",           10, 5},
		{TokenType::Return,           "return",      11, 5},
		{TokenType::Identifier,       "sum",         11, 12},
		{TokenType::Semicolon,        ";",           11, 15},
		{TokenType::RightBrace,       "}",           12, 1},
		{TokenType::Eof,              "",            13, 1}
	};

	// Act
	std::vector<Token> actual_tokens = lexer.tokenize();

	// Assert
	ASSERT_EQ(expected_tokens.size(), actual_tokens.size());
	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		EXPECT_EQ(expected_tokens[i], actual_tokens[i]);
	}
}