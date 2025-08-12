#include <aera/source_context.hpp>
#include <gtest/gtest.h>

TEST(SourceContextTest, RegularFileWithTrailingNewline) {
    // Arrange
    std::string src = "print(\"Hello\")\n"
        "x = 42\n"
        "quit()\n";
    auto ctx = aera::SourceContext("test.aera", src);

    // Act & Assert
    EXPECT_EQ(ctx.get_line(0), "print(\"Hello\")");
    EXPECT_EQ(ctx.get_line(1), "x = 42");
    EXPECT_EQ(ctx.get_line(2), "quit()");
}

TEST(SourceContextTest, NoTrailingNewline) {
    // Arrange
    std::string src = "first line\n"
        "second line\n"
        "third line";
    auto ctx = aera::SourceContext("test.aera", src);

    // Act & Assert
    EXPECT_EQ(ctx.get_line(0), "first line");
    EXPECT_EQ(ctx.get_line(1), "second line");
    EXPECT_EQ(ctx.get_line(2), "third line");
}

TEST(SourceContextTest, SingleLine) {
    // Arrange
    std::string src = "only one line";
    auto ctx = aera::SourceContext("test.aera", src);

    // Act & Assert
    EXPECT_EQ(ctx.get_line(0), "only one line");
}

TEST(SourceContextTest, EmptyFile) {
    // Arrange
    std::string src = "";
    auto ctx = aera::SourceContext("test.aera", src);

    // Act & Assert
    EXPECT_TRUE(ctx.source().empty());
    EXPECT_THROW(ctx.get_line(0), std::out_of_range); // no lines expected, issue with lexer / parser
}

TEST(SourceContextTest, HandlesMultipleConsecutiveNewlines) {
    // Arrange
    std::string src = "line 1\n\nline 3";
    auto ctx = aera::SourceContext("test.aera", src);

    // Act & Assert
    EXPECT_EQ(ctx.get_line(0), "line 1");
    EXPECT_EQ(ctx.get_line(1), ""); // blank line
    EXPECT_EQ(ctx.get_line(2), "line 3");
}
