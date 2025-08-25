#include <aera/lexer/lexer.hpp>
#include <test_helpers.hpp>

// Basic Token Recognition

TEST(LexerTest, TokenizesLetStatement) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let x = 5"; // defaults to int64

	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("x", 1, 5),
		tf.equal(1, 7),
		tf.int_lit("5", static_cast<int64_t>(5), 1, 9),
		tf.eof(1, 10)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesIfStatement) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "if true { do_something() }";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.if_stmt(1, 1),
		tf.true_lit(1, 4),
		tf.left_brace(1, 9),
		tf.id("do_something", 1, 11),
		tf.left_paren(1, 23),
		tf.right_paren(1, 24),
		tf.right_brace(1, 26),
		tf.eof(1, 27)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		std::cout << i << ": " << actual_tokens[i].lexeme << std::endl;
	}

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesWhileStatement) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "while x > 10 { do_something() }";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);


	std::vector<aera::Token> expected_tokens = {
		tf.while_stmt(1, 1),
		tf.id("x", 1, 7),
		tf.greater(1, 9),
		tf.int_lit("10", static_cast<int64_t>(10), 1, 11),
		tf.left_brace(1, 14),
		tf.id("do_something", 1, 16),
		tf.left_paren(1, 28),
		tf.right_paren(1, 29),
		tf.right_brace(1, 31),
		tf.eof(1, 32)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesForStatement_Exclusive) {
	// Arrange
	std::string filename = "main.aera";

	std::string input = "for i in 0..10 { do_something() }";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.for_stmt(1, 1),
		tf.id("i", 1, 5),
		tf.in(1, 7),
		tf.int_lit("0", static_cast<int64_t>(0), 1, 10),
		tf.period_period(1, 11),
		tf.int_lit("10", static_cast<int64_t>(10), 1, 13),
		tf.left_brace(1, 16),
		tf.id("do_something", 1, 18),
		tf.left_paren(1, 30),
		tf.right_paren(1, 31),
		tf.right_brace(1, 33),
		tf.eof(1, 34)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesForStatement_Inclusive) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "for i in 0..=10 { do_something() }";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.for_stmt(1, 1),
		tf.id("i", 1, 5),
		tf.in(1, 7),
		tf.int_lit("0", static_cast<int64_t>(0), 1, 10),
		tf.period_period_equal(1, 11),
		tf.int_lit("10", static_cast<int64_t>(10), 1, 14),
		tf.left_brace(1, 17),
		tf.id("do_something", 1, 19),
		tf.left_paren(1, 31),
		tf.right_paren(1, 32),
		tf.right_brace(1, 34),
		tf.eof(1, 35)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesLooptatement) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "loop { do_something() }";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);


	std::vector<aera::Token> expected_tokens = {
		tf.loop(1, 1),
		tf.left_brace(1, 6),
		tf.id("do_something", 1, 8),
		tf.left_paren(1, 20),
		tf.right_paren(1, 21),
		tf.right_brace(1, 23),
		tf.eof(1, 24)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesFunctionDeclaration) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "pub fn add(a: int32, b: int32) -> int32 { return a + b }";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.pub(1, 1),
		tf.fn(1, 5),
		tf.id("add", 1, 8),
		tf.left_paren(1, 11),
		tf.id("a", 1, 12),
		tf.colon(1, 13),
		tf.id("int32", 1, 15),
		tf.comma(1, 20),
		tf.id("b", 1, 22),
		tf.colon(1, 23),
		tf.id("int32", 1, 25),
		tf.right_paren(1, 30),
		tf.minus_greater(1, 32),
		tf.id("int32", 1, 35),
		tf.left_brace(1, 41),
		tf.return_stmt(1, 43),
		tf.id("a", 1, 50),
		tf.plus(1, 52),
		tf.id("b", 1, 54),
		tf.right_brace(1, 56),
		tf.eof(1, 57)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesMultiSentence) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = R"(let val = 10
# A comment to skip
  fn_call()
)";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("val", 1, 5),
		tf.equal(1, 9),
		tf.int_lit("10", static_cast<int64_t>(10), 1, 11),
		tf.newline(2, 1),
		tf.newline(3, 1),
		tf.id("fn_call", 3, 3),
		tf.left_paren(3, 10),
		tf.right_paren(3, 11),
		tf.newline(4, 1),
		tf.eof(4, 1)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

// Edge Cases and Boundary Conditions

TEST(LexerTest, EmptyInput_ProducesOnlyEof) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.eof(1, 1)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, WhitespaceOnlyInput_ProducesOnlyNewlineAndEof) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = " \t \r\n   ";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.newline(2, 1),
		tf.eof(2, 4)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

// String Literals

TEST(LexerTest, TokenizesStringWithNewlineEscape) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let s = \"hello\\nworld\"";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("s", 1, 5),
		tf.equal(1, 7),
		tf.string_lit("hello\nworld", 1, 9),
		tf.eof(1, 23)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesEmptyString) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let empty = \"\"";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("empty", 1, 5),
		tf.equal(1, 11),
		tf.string_lit("", 1, 13),
		tf.eof(1, 15)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, HandlesEscapedQuotesAndBackslashes) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let s = \"a \\\"quoted\\\" string and a \\\\ backslash\"";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("s", 1, 5),
		tf.equal(1, 7),
		tf.string_lit("a \"quoted\" string and a \\ backslash", 1, 9),
		tf.eof(1, 49)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, HandlesUnterminatedString) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let s = \"abc";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("s", 1, 5),
		tf.equal(1, 7),
		tf.illegal("\"abc", 1, 9),
		tf.eof(1, 13)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, HandlesInvalidEscapeInString) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "\"hello \\z world\"";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.illegal("\"hello \\z world\"", 1, 1),
		tf.eof(1, 17)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

// Character Literals

TEST(LexerTest, TokenizesSimpleCharacter) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let c = 'a'";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("c", 1, 5),
		tf.equal(1, 7),
		tf.character_lit("a", 'a', 1, 9),
		tf.eof(1, 12)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesEscapedCharacter) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let newline = '\\n'";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("newline", 1, 5),
		tf.equal(1, 13),
		tf.character_lit("\n", '\n', 1, 15),
		tf.eof(1, 19)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, HandlesEmptyCharacterLiteral) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let c = ''";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("c", 1, 5),
		tf.equal(1, 7),
		tf.illegal("''", 1, 9),
		tf.eof(1, 11)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, HandlesTooLongCharacterLiteral) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let c = 'ab'";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("c", 1, 5),
		tf.equal(1, 7),
		tf.illegal("'ab'", 1, 9),
		tf.eof(1, 13)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

// Numeric Literals

TEST(LexerTest, TokenizesBinaryIntegerLiterals) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let answer = 0b001";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("answer", 1, 5),
		tf.equal(1, 12),
		tf.int_lit("0b001", static_cast<int64_t>(0b001), 1, 14),
		tf.eof(1, 19)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesOctalIntegerLiterals) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let answer = 0o512";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("answer", 1, 5),
		tf.equal(1, 12),
		tf.int_lit("0o512", static_cast<int64_t>(0512), 1, 14),
		tf.eof(1, 19)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesHexadecimalIntegerLiterals) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let answer = 0xFF0000";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("answer", 1, 5),
		tf.equal(1, 12),
		tf.int_lit("0xFF0000", static_cast<int64_t>(0xFF0000), 1, 14),
		tf.eof(1, 22)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	std::cout << "Actual token count: " << actual_tokens.size() << std::endl;
	for (size_t i = 0; i < actual_tokens.size(); ++i) {
		std::cout << i << ": " << actual_tokens[i].lexeme
			<< " at line " << actual_tokens[i].loc.line
			<< ", col " << actual_tokens[i].loc.col << std::endl;
	}

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesDecimalIntegerLiterals) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let answer = 42";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("answer", 1, 5),
		tf.equal(1, 12),
		tf.int_lit("42", static_cast<int64_t>(42), 1, 14),
		tf.eof(1, 16)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesFloatLiterals) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let pi = 3.14";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("pi", 1, 5),
		tf.equal(1, 8),
		tf.float_lit("3.14", static_cast<double>(3.14), 1, 10),
		tf.eof(1, 14)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesFloatScientificLiterals) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let val = 1.7e12";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("val", 1, 5),
		tf.equal(1, 9),
		tf.float_lit("1.7e12", static_cast<double>(1.7e12), 1, 11),
		tf.eof(1, 17)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}


TEST(LexerTest, TokenizesFloatScientificPositiveLiterals) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let e = 2.7e+5";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("e", 1, 5),
		tf.equal(1, 7),
		tf.float_lit("2.7e+5", static_cast<double>(2.7e+5), 1, 9),
		tf.eof(1, 15)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesFloatScientificNegativeLiterals) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let lr = 0.1e-5";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("lr", 1, 5),
		tf.equal(1, 8),
		tf.float_lit("0.1e-5", static_cast<double>(0.1e-5), 1, 10),
		tf.eof(1, 16)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesFloatWithTrailingDot) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let pi = 3.";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("pi", 1, 5),
		tf.equal(1, 8),
		tf.float_lit("3.", static_cast<double>(3.), 1, 10),
		tf.eof(1, 12)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, HandlesMalformedNumberWithExtraDot) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let pi = 3.14.";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("pi", 1, 5),
		tf.equal(1, 8),
		tf.illegal("3.14.", 1, 10),
		tf.eof(1, 15)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, HandlesInvalidFloat_InForStatement) {
	// Arrange
	std::string filename = "main.aera";

	std::string input = "for i in 1.0..10.0 { do_something() }";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.for_stmt(1, 1),
		tf.id("i", 1, 5),
		tf.in(1, 7),
		tf.illegal("1.0..", 1, 10),
		tf.float_lit("10.0", static_cast<double>(10.0), 1, 15),
		tf.left_brace(1, 20),
		tf.id("do_something", 1, 22),
		tf.left_paren(1, 34),
		tf.right_paren(1, 35),
		tf.right_brace(1, 37),
		tf.eof(1, 38)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

// Comments

TEST(LexerTest, LineCommentOnly_ProducesOnlyEof) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "# this is a comment and should be ignored";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.eof(1, 42)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, BlockCommentOnly_ProducesOnlyEof) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "<# this is a block comment and should be ignored #>";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.eof(1, 52)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokensAndComments_IgnoresComments) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let x = 5 # assign 5 to x";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("x", 1, 5),
		tf.equal(1, 7),
		tf.int_lit("5", static_cast<int64_t>(5), 1, 9),
		tf.eof(1, 26)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, HandlesUnterminatedBlockComment) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let x = 5 <# this is not closed";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("x", 1, 5),
		tf.equal(1, 7),
		tf.int_lit("5", static_cast<int64_t>(5), 1, 9),
		tf.illegal("<# this is not closed", 1, 11),
		tf.eof(1, 32)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, HandlesMalformedBlockCommentTerminator) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "<# this is not closed again #";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.illegal("<# this is not closed again #", 1, 1),
		tf.eof(1, 30)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

// Keywords vs Identifiers

TEST(LexerTest, DistinguishesKeywordsFromIdentifiers) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "if ifdef { let return_value = true }";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.if_stmt(1, 1),
		tf.id("ifdef", 1, 4),
		tf.left_brace(1, 10),
		tf.let(1, 12),
		tf.id("return_value", 1, 16),
		tf.equal(1, 29),
		tf.true_lit(1, 31),
		tf.right_brace(1, 36),
		tf.eof(1, 37)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

// Error Handling

TEST(LexerTest, HandlesInvalidCharacter) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = "let x = $";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.let(1, 1),
		tf.id("x", 1, 5),
		tf.equal(1, 7),
		tf.illegal("$", 1, 9),
		tf.eof(1, 10)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

// Multi-Line Examples

TEST(LexerTest, TokenizesHelloWorldProgram) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = R"(fn main() {
    print("Hello world")
}
)";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		tf.fn(1, 1),
		tf.id("main", 1, 4),
		tf.left_paren(1, 8),
		tf.right_paren(1, 9),
		tf.left_brace(1, 11),
		tf.newline(2, 1),
		tf.id("print", 2, 5),
		tf.left_paren(2, 10),
		tf.string_lit("Hello world", 2, 11),
		tf.right_paren(2, 24),
		tf.newline(3, 1),
		tf.right_brace(3, 1),
		tf.newline(4, 1),
		tf.eof(4, 1)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}

TEST(LexerTest, TokenizesComplexFunctionAndLoop) {
	// Arrange
	std::string filename = "main.aera";
	std::string input = R"(fn main() {
    let ans = sum(5)
    print(ans)
}

fn sum(num: int64) -> int64 {
    let mut total: int64 = 0
    for i in 0..num {
        total += i
    }
    return total
}
)";
	aera::DiagnosticReporter diagnostics;
	aera::SourceContext context(filename, input);
	aera::lexer::Lexer lexer(context, diagnostics);
	aera::test::TokenFactory tf(filename);

	std::vector<aera::Token> expected_tokens = {
		// main function
		tf.fn(1, 1),
		tf.id("main", 1, 4),
		tf.left_paren(1, 8),
		tf.right_paren(1, 9),
		tf.left_brace(1, 11),
		tf.newline(2, 1),
		tf.let(2, 5),
		tf.id("ans", 2, 9),
		tf.equal(2, 13),
		tf.id("sum", 2, 15),
		tf.left_paren(2, 18),
		tf.int_lit("5", static_cast<int64_t>(5), 2, 19),
		tf.right_paren(2, 20),
		tf.newline(3, 1),
		tf.id("print", 3, 5),
		tf.left_paren(3, 10),
		tf.id("ans", 3, 11),
		tf.right_paren(3, 14),
		tf.newline(4, 1),
		tf.right_brace(4, 1),
		tf.newline(5, 1),
		tf.newline(6, 1),

		// sum function
		tf.fn(6, 1),
		tf.id("sum", 6, 4),
		tf.left_paren(6, 7),
		tf.id("num", 6, 8),
		tf.colon(6, 11),
		tf.id("int64", 6, 13),
		tf.right_paren(6, 18),
		tf.minus_greater(6, 20),
		tf.id("int64", 6, 23),
		tf.left_brace(6, 29),
		tf.newline(7, 1),
		tf.let(7, 5),
		tf.mut(7, 9),
		tf.id("total", 7, 13),
		tf.colon(7, 18),
		tf.id("int64", 7, 20),
		tf.equal(7, 26),
		tf.int_lit("0", static_cast<int64_t>(0), 7, 28),
		tf.newline(8, 1),
		tf.for_stmt(8, 5),
		tf.id("i", 8, 9),
		tf.in(8, 11),
		tf.int_lit("0", static_cast<int64_t>(0), 8, 14),
		tf.period_period(8, 15),
		tf.id("num", 8, 17),
		tf.left_brace(8, 21),
		tf.newline(9, 1),
		tf.id("total", 9, 9),
		tf.plus_equal(9, 15),
		tf.id("i", 9, 18),
		tf.newline(10, 1),
		tf.right_brace(10, 5),
		tf.newline(11, 1),
		tf.return_stmt(11, 5),
		tf.id("total", 11, 12),
		tf.newline(12, 1),
		tf.right_brace(12, 1),
		tf.newline(13, 1),
		tf.eof(13, 1)
	};

	// Act
	std::vector<aera::Token> actual_tokens = lexer.tokenize();

	// Assert
	aera::test::expect_tokens(actual_tokens, expected_tokens);
}
