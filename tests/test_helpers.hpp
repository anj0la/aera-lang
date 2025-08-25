#pragma once
#include <gtest/gtest.h>
#include <aera/token.hpp>
#include <aera/source_location.hpp>
#include <vector>
#include <string>

namespace aera::test {

    inline aera::Token make_token(aera::TokenType type, const std::string& lexeme, const aera::Value& value,
        int line, int col, const std::string& filename = "test.aera") {
        return aera::Token{ type, lexeme, aera::SourceLocation{filename, line, col}, value };
    }

    class TokenFactory {
    public:
        explicit TokenFactory(const std::string& file = "test.aera") : filename(file) {}

        aera::Token make(aera::TokenType type, const std::string& lexeme, const aera::Value& value, int line, int col) {
            return aera::Token{ type, lexeme, aera::SourceLocation{filename, line, col}, value };
        }

        aera::Token make(aera::TokenType type, const std::string& lexeme, int line, int col) {
            return aera::Token{ type, lexeme, aera::SourceLocation{filename, line, col}, lexeme };
        }

        // Common token shortcuts
   
        // Punctuation

        aera::Token left_paren(int line, int col) { return make(aera::TokenType::LeftParen, "(", line, col); }
        aera::Token right_paren(int line, int col) { return make(aera::TokenType::RightParen, ")", line, col); }
        aera::Token left_brace(int line, int col) { return make(aera::TokenType::LeftBrace, "{", line, col); }
        aera::Token right_brace(int line, int col) { return make(aera::TokenType::RightBrace, "}", line, col); }
        aera::Token left_bracket(int line, int col) { return make(aera::TokenType::LeftBracket, "[", line, col); }
        aera::Token right_bracket(int line, int col) { return make(aera::TokenType::RightBracket, "]", line, col); }
        aera::Token comma(int line, int col) { return make(aera::TokenType::Comma, ",", line, col); }
        aera::Token period(int line, int col) { return make(aera::TokenType::Period, ".", line, col); }
        aera::Token semicolon(int line, int col) { return make(aera::TokenType::Semicolon, ";", line, col); }
        aera::Token colon(int line, int col) { return make(aera::TokenType::Colon, ":", line, col); }
        aera::Token grave(int line, int col) { return make(aera::TokenType::Grave, "`", line, col); }
        aera::Token newline(int line, int col) { return make(aera::TokenType::Newline, "\n", '\n', line, col); }

        // Operators

        aera::Token less_less_equal(int line, int col) { return make(aera::TokenType::LessLessEqual, "<<=", line, col); }
        aera::Token greater_greater_equal(int line, int col) { return make(aera::TokenType::GreaterGreaterEqual, ">>=", line, col); }
        aera::Token period_period_equal(int line, int col) { return make(aera::TokenType::PeriodPeriodEqual, "..=", line, col); }
        aera::Token amp_amp(int line, int col) { return make(aera::TokenType::AmpAmp, "&&", line, col); }
        aera::Token pipe_pipe(int line, int col) { return make(aera::TokenType::PipePipe, "||", line, col); }
        aera::Token equal_equal(int line, int col) { return make(aera::TokenType::EqualEqual, "==", line, col); }
        aera::Token greater_equal(int line, int col) { return make(aera::TokenType::GreaterEqual, ">=", line, col); }
        aera::Token less_equal(int line, int col) { return make(aera::TokenType::LessEqual, "<=", line, col); }
        aera::Token greater_greater(int line, int col) { return make(aera::TokenType::GreaterGreater, ">>", line, col); }
        aera::Token less_less(int line, int col) { return make(aera::TokenType::LessLess, "<<", line, col); }
        aera::Token plus_plus(int line, int col) { return make(aera::TokenType::PlusPlus, "++", line, col); }
        aera::Token minus_minus(int line, int col) { return make(aera::TokenType::MinusMinus, "--", line, col); }
        aera::Token plus_equal(int line, int col) { return make(aera::TokenType::PlusEqual, "+=", line, col); }
        aera::Token minus_equal(int line, int col) { return make(aera::TokenType::MinusEqual, "-=", line, col); }
        aera::Token star_equal(int line, int col) { return make(aera::TokenType::StarEqual, "*=", line, col); }
        aera::Token slash_equal(int line, int col) { return make(aera::TokenType::SlashEqual, "/=", line, col); }
        aera::Token percent_equal(int line, int col) { return make(aera::TokenType::PercentEqual, "%=", line, col); }
        aera::Token amp_equal(int line, int col) { return make(aera::TokenType::AmpEqual, "&=", line, col); }
        aera::Token pipe_equal(int line, int col) { return make(aera::TokenType::PipeEqual, "|=", line, col); }
        aera::Token caret_equal(int line, int col) { return make(aera::TokenType::CaretEqual, "^=", line, col); }
        aera::Token tilde_equal(int line, int col) { return make(aera::TokenType::TildeEqual, "~=", line, col); }
        aera::Token minus_greater(int line, int col) { return make(aera::TokenType::MinusGreater, "->", line, col); }
        aera::Token period_period(int line, int col) { return make(aera::TokenType::PeriodPeriod, "..", line, col); }
        aera::Token equal_greater(int line, int col) { return make(aera::TokenType::EqualGreater, "=>", line, col); }
        aera::Token question_question(int line, int col) { return make(aera::TokenType::QuestionQuestion, "??", line, col); }

        aera::Token amp(int line, int col) { return make(aera::TokenType::Amp, "&", line, col); }
        aera::Token pipe(int line, int col) { return make(aera::TokenType::At, "|", line, col); }
        aera::Token caret(int line, int col) { return make(aera::TokenType::Caret, "^", line, col); }
        aera::Token tilde(int line, int col) { return make(aera::TokenType::Tilde, "~", line, col); }

        aera::Token plus(int line, int col) { return make(aera::TokenType::Plus, "+", line, col); }
        aera::Token minus(int line, int col) { return make(aera::TokenType::Minus, "-", line, col); }
        aera::Token star(int line, int col) { return make(aera::TokenType::Star, "*", line, col); }
        aera::Token slash(int line, int col) { return make(aera::TokenType::Slash, "/", line, col); }
        aera::Token percent(int line, int col) { return make(aera::TokenType::Percent, "%", line, col); }
        aera::Token question(int line, int col) { return make(aera::TokenType::Question, "?", line, col); }
        aera::Token at(int line, int col) { return make(aera::TokenType::At, "@", line, col); }
        aera::Token exclaim(int line, int col) { return make(aera::TokenType::Exclaim, "!", line, col); }
        aera::Token equal(int line, int col) { return make(aera::TokenType::Equal, "=", line, col); }
        aera::Token greater(int line, int col) { return make(aera::TokenType::Greater, ">", line, col); }
        aera::Token less(int line, int col) { return make(aera::TokenType::Less, "<", line, col); }

        // Literals

        aera::Token int_lit(const std::string& lexeme, const aera::Value& value, int line, int col) { return make(aera::TokenType::IntLiteral, lexeme, value, line, col); }
        aera::Token float_lit(const std::string& lexeme, const aera::Value& value, int line, int col) { return make(aera::TokenType::FloatLiteral, lexeme, value, line, col); }
        aera::Token character_lit(const std::string& lexeme, const aera::Value& value, int line, int col) { return make(aera::TokenType::CharacterLiteral, lexeme, value, line, col); }
        aera::Token string_lit(const std::string& lexeme, int line, int col) { return make(aera::TokenType::StringLiteral, lexeme, lexeme, line, col); }
        aera::Token true_lit(int line, int col) { return make(aera::TokenType::True, "true", true, line, col); }
        aera::Token false_lit(int line, int col) { return make(aera::TokenType::False, "false", false, line, col); }

        // Keywords

        aera::Token let(int line, int col) { return make(aera::TokenType::Let, "let", line, col); }
        aera::Token fn(int line, int col) { return make(aera::TokenType::Fn, "fn", line, col); }
        aera::Token pub(int line, int col) { return make(aera::TokenType::Pub, "pub", line, col); }
        aera::Token mut(int line, int col) { return make(aera::TokenType::Mut, "mut", line, col); }
        aera::Token modifies(int line, int col) { return make(aera::TokenType::Modifies, "modifies", line, col); }

        aera::Token return_stmt(int line, int col) { return make(aera::TokenType::Return, "return", line, col); }
        aera::Token if_stmt(int line, int col) { return make(aera::TokenType::If, "if", line, col); }
        aera::Token else_stmt(int line, int col) { return make(aera::TokenType::Else, "else", line, col); }
        aera::Token while_stmt(int line, int col) { return make(aera::TokenType::While, "while", line, col); }
        aera::Token for_stmt(int line, int col) { return make(aera::TokenType::For, "for", line, col); }
        aera::Token loop(int line, int col) { return make(aera::TokenType::Loop, "loop", line, col); }

        aera::Token class_stmt(int line, int col) { return make(aera::TokenType::Class, "class", line, col); }
        aera::Token struct_stmt(int line, int col) { return make(aera::TokenType::Struct, "struct", line, col); }
        aera::Token trait(int line, int col) { return make(aera::TokenType::Trait, "trait", line, col); }
        aera::Token with(int line, int col) { return make(aera::TokenType::With, "with", line, col); }
        aera::Token as(int line, int col) { return make(aera::TokenType::As, "as", line, col); }
        aera::Token in(int line, int col) { return make(aera::TokenType::In, "in", line, col); }

        // Identifier

        aera::Token id(const std::string& name, int line, int col) { return make(aera::TokenType::Identifier, name, line, col); }

        // Errors

        aera::Token illegal(const std::string& name, int line, int col) { return make(aera::TokenType::Illegal, name, line, col); }
        aera::Token eof(int line, int col) { return make(aera::TokenType::Eof, "", line, col); }

    private:
        std::string filename;
    };

    inline void expect_tokens(const std::vector<aera::Token>& actual,
        const std::vector<aera::Token>& expected) {
        ASSERT_EQ(actual.size(), expected.size()) << "Token count mismatch";

        for (size_t i = 0; i < actual.size(); ++i) {
            EXPECT_EQ(actual[i].type, expected[i].type)
                << "Token " << i << " type mismatch";
            EXPECT_EQ(actual[i].value, expected[i].value)
                << "Token " << i << " value mismatch";
            EXPECT_EQ(actual[i].loc.line, expected[i].loc.line)
                << "Token " << i << " line mismatch";
            EXPECT_EQ(actual[i].loc.col, expected[i].loc.col)
                << "Token " << i << " column mismatch";
        }
    }
}