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
		const auto& expected = expected_tokens[i];
		const auto& actual = actual_tokens[i];

		EXPECT_EQ(expected, actual);
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
		const auto& expected = expected_tokens[i];
		const auto& actual = actual_tokens[i];

		EXPECT_EQ(expected, actual);
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
		const auto& expected = expected_tokens[i];
		const auto& actual = actual_tokens[i];

		EXPECT_EQ(expected, actual);
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
		const auto& expected = expected_tokens[i];
		const auto& actual = actual_tokens[i];

		EXPECT_EQ(expected, actual);
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
		const auto& expected = expected_tokens[i];
		const auto& actual = actual_tokens[i];

		EXPECT_EQ(expected, actual);
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


// Numeric Literals


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
		const auto& expected = expected_tokens[i];
		const auto& actual = actual_tokens[i];

		EXPECT_EQ(expected, actual);
	}
}

// Keywords vs Identifiers


// Error Handling


// Performance and Memory


