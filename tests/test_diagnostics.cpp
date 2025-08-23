#include <gtest/gtest.h>
#include <sstream>
#include <iostream>
#include <aera/diagnostics.hpp>
#include <aera/source_location.hpp>

class OutputCapture {
public:
    OutputCapture() : old_cerr(std::cerr.rdbuf()) {
        std::cerr.rdbuf(buffer.rdbuf());
    }

    ~OutputCapture() {
        std::cerr.rdbuf(old_cerr);
    }

    std::string get_output() {
        return buffer.str();
    }

    void clear() {
        buffer.str("");
        buffer.clear();
    }

private:
    std::ostringstream buffer;
    std::streambuf* old_cerr;
};

class DiagnosticReporterTest : public ::testing::Test {
protected:
    std::unique_ptr<aera::DiagnosticReporter> reporter;
    OutputCapture capture;

    void SetUp() override {
        reporter = std::make_unique<aera::DiagnosticReporter>();
    }
};

TEST_F(DiagnosticReporterTest, DebugCapture) {
    // Test 1: Direct cout
    std::cout << "Direct cout test" << std::endl;
    std::string output1 = capture.get_output();
    std::cout << "Captured direct: length=" << output1.length() << ", content='" << output1 << "'" << std::endl;

    capture.clear();

    // Test 2: After clear
    std::cout << "After clear test" << std::endl;
    std::string output2 = capture.get_output();
    std::cout << "Captured after clear: length=" << output2.length() << ", content='" << output2 << "'" << std::endl;
}

// State Tracking Tests

TEST_F(DiagnosticReporterTest, InitialStateIsEmpty) {
    // Assert
    EXPECT_FALSE(reporter->has_errors());
    EXPECT_FALSE(reporter->has_warnings());
    EXPECT_EQ(0u, reporter->error_count());
    EXPECT_EQ(0u, reporter->warning_count());
}

TEST_F(DiagnosticReporterTest, AddErrorUpdatesState) {
    // Arrange
    aera::SourceLocation loc{"test.aera", 1, 5 };

    // Act
    reporter->add_error(loc.filepath, 0, loc, "test error message");

    // Assert
    EXPECT_TRUE(reporter->has_errors());
    EXPECT_FALSE(reporter->has_warnings());
    EXPECT_EQ(1u, reporter->error_count());
    EXPECT_EQ(0u, reporter->warning_count());
}

TEST_F(DiagnosticReporterTest, AddWarningUpdatesState) {
    // Arrange
    aera::SourceLocation loc{"test.aera", 2, 10 };

    // Act
    reporter->add_warning(loc.filepath, 0, loc, "test warning message");

    // Assert
    EXPECT_FALSE(reporter->has_errors());
    EXPECT_TRUE(reporter->has_warnings());
    EXPECT_EQ(0u, reporter->error_count());
    EXPECT_EQ(1u, reporter->warning_count());
}

TEST_F(DiagnosticReporterTest, AddNoteDoesNotUpdateErrorWarningCounts) {
    // Arrange
    aera::SourceLocation loc{"test.aera", 3, 15 };

    // Act
    reporter->note(loc.filepath, 0, loc, "test note message");

    // Assert
    EXPECT_FALSE(reporter->has_errors());
    EXPECT_FALSE(reporter->has_warnings());
    EXPECT_EQ(0u, reporter->error_count());
    EXPECT_EQ(0u, reporter->warning_count());
}

TEST_F(DiagnosticReporterTest, MultipleErrorsAndWarnings) {
    // Arrange
    aera::SourceLocation loc1{"test.aera", 1, 5};
    aera::SourceLocation loc2{"test.aera", 2, 10 };
    aera::SourceLocation loc3{"test.aera", 3, 15 };
    aera::SourceLocation loc4{"test.aera", 4, 20 };

    // Act
    reporter->add_error(loc1.filepath, 0, loc1, "error 1");
    reporter->add_warning(loc2.filepath, 0, loc2, "warning 1");
    reporter->add_error(loc3.filepath, 0, loc3, "error 2");
    reporter->add_warning(loc4.filepath, 0, loc4, "warning 2");

    // Assert
    EXPECT_TRUE(reporter->has_errors());
    EXPECT_TRUE(reporter->has_warnings());
    EXPECT_EQ(2u, reporter->error_count());
    EXPECT_EQ(2u, reporter->warning_count());
}

TEST_F(DiagnosticReporterTest, MixedDiagnosticsWithNotes) {
    // Arrange
    aera::SourceLocation loc1{"test.aera", 1, 5 };
    aera::SourceLocation loc2{"test.aera", 2, 10 };
    aera::SourceLocation loc3{"test.aera", 3, 15 };

    // Act
    reporter->add_error(loc1.filepath, 0, loc1, "error message");
    reporter->note(loc2.filepath, 0, loc2, "note message");
    reporter->add_warning(loc3.filepath, 0, loc3, "warning message");

    // Assert
    EXPECT_TRUE(reporter->has_errors());
    EXPECT_TRUE(reporter->has_warnings());
    EXPECT_EQ(1u, reporter->error_count());
    EXPECT_EQ(1u, reporter->warning_count());
}

// Output Formatting Tests

TEST_F(DiagnosticReporterTest, PrintErrorBasicFormatting) {
    // Arrange
    aera::SourceLocation loc{"main.aera", 1, 1 };
    std::string source_line = "<# a block comment without its enclosing characters";
    int token_length = 2;

    // Act
    reporter->add_error(loc.filepath, token_length, loc, "unterminated block comment", source_line, "block comments are enclosed with #>");
    capture.clear();
    reporter->print_errors();
    std::string output = capture.get_output();

    // Assert
    EXPECT_NE(output.find("main.aera"), std::string::npos);
    EXPECT_NE(output.find("1:1"), std::string::npos);
    EXPECT_NE(output.find("error"), std::string::npos);
    EXPECT_NE(output.find("unterminated block comment"), std::string::npos);
    EXPECT_NE(output.find(source_line), std::string::npos);
    EXPECT_NE(output.find("^"), std::string::npos);
    EXPECT_NE(output.find("block comments are enclosed with #>"), std::string::npos);
}

TEST_F(DiagnosticReporterTest, PrintWarningBasicFormatting) {
    // Arrange
    aera::SourceLocation loc{"main.aera", 5, 5 };
    std::string source_line = "let x: int32 = 5";
    int token_length = 1;

    // Act
    reporter->add_warning(loc.filepath, token_length, loc, "unused variable", source_line);
    capture.clear();
    reporter->print_warnings();
    std::string output = capture.get_output();

    // Assert
    EXPECT_NE(output.find("main.aera"), std::string::npos);
    EXPECT_NE(output.find("5:5"), std::string::npos);
    EXPECT_NE(output.find("warning"), std::string::npos);
    EXPECT_NE(output.find("unused variable"), std::string::npos);
    EXPECT_NE(output.find(source_line), std::string::npos);
    EXPECT_NE(output.find("^"), std::string::npos);
}

TEST_F(DiagnosticReporterTest, PrintNoteBasicFormatting) {
    // Arrange
    aera::SourceLocation loc{"main.aera", 5, 5 };
    std::string source_line = "let userName: string = \"annie\"";
    int token_length = 8;

    // Act
    reporter->note(loc.filepath, token_length, loc, "variable name uses camelCase, consider snake_case: user_name", source_line);
    capture.clear();
    reporter->print_notes();
    std::string output = capture.get_output();

    // Assert
    EXPECT_NE(output.find("main.aera"), std::string::npos);
    EXPECT_NE(output.find("5:5"), std::string::npos);
    EXPECT_NE(output.find("note"), std::string::npos);
    EXPECT_NE(output.find("variable name uses camelCase, consider snake_case: user_name"), std::string::npos);
    EXPECT_NE(output.find(source_line), std::string::npos);
    EXPECT_NE(output.find("^"), std::string::npos);
}

TEST_F(DiagnosticReporterTest, CaretAlignmentTest) {
    // Arrange
    aera::SourceLocation loc{"main.aera", 1, 9 };
    std::string source_line = "println(\"hello world)";
    int token_length = 13;

    // Act
    reporter->add_error(loc.filepath, token_length, loc, "unterminated string literal", source_line);
    capture.clear();
    reporter->print_errors();
    std::string output = capture.get_output();

    // Assert
    size_t source_pos = output.find(source_line);
    ASSERT_NE(source_pos, std::string::npos);

    size_t next_line = output.find('\n', source_pos);
    ASSERT_NE(next_line, std::string::npos);

    std::string caret_line = output.substr(next_line + 1);
    size_t caret_end = caret_line.find('\n');
    if (caret_end != std::string::npos) {
        caret_line = caret_line.substr(0, caret_end);
    }

    EXPECT_NE(caret_line.find("^"), std::string::npos);

    size_t caret_pos = caret_line.find("^");
    ASSERT_NE(caret_pos, std::string::npos);
    EXPECT_EQ(caret_pos, static_cast<size_t>(4 + loc.col - 1)); // add 4 to consider space
}

// Print Filtering Tests

TEST_F(DiagnosticReporterTest, PrintErrorsOnlyPrintsErrors) {
    // Arrange
    aera::SourceLocation loc{"test.aera", 1, 5 };

    // Act
    reporter->add_error(loc.filepath, 0, loc, "error message");
    reporter->add_warning(loc.filepath, 0, loc, "warning message");
    reporter->note(loc.filepath, 0, loc, "note message");
    capture.clear();
    reporter->print_errors();
    std::string output = capture.get_output();

    // Assert
    EXPECT_NE(output.find("error message"), std::string::npos);
    EXPECT_EQ(output.find("warning message"), std::string::npos);
    EXPECT_EQ(output.find("note message"), std::string::npos);
}

TEST_F(DiagnosticReporterTest, PrintWarningsOnlyPrintsWarnings) {
    // Arrange
    aera::SourceLocation loc{"test.aera", 1, 5 };

    // Act
    reporter->add_error(loc.filepath, 0, loc, "error message");
    reporter->add_warning(loc.filepath, 0, loc, "warning message");
    reporter->note(loc.filepath, 0, loc, "note message");
    capture.clear();
    reporter->print_warnings();
    std::string output = capture.get_output();

    // Assert
    EXPECT_EQ(output.find("error message"), std::string::npos);
    EXPECT_NE(output.find("warning message"), std::string::npos);
    EXPECT_EQ(output.find("note message"), std::string::npos);
}

TEST_F(DiagnosticReporterTest, PrintNotesOnlyPrintsNotes) {
    // Arrange
    aera::SourceLocation loc{"test.aera", 1, 5 };

    // Act
    reporter->add_error(loc.filepath, 0, loc, "error message");
    reporter->add_warning(loc.filepath, 0, loc, "warning message");
    reporter->note(loc.filepath, 0, loc, "note message");
    capture.clear();
    reporter->print_notes();
    std::string output = capture.get_output();

    // Assert
    EXPECT_EQ(output.find("error message"), std::string::npos);
    EXPECT_EQ(output.find("warning message"), std::string::npos);
    EXPECT_NE(output.find("note message"), std::string::npos);
}

TEST_F(DiagnosticReporterTest, PrintAllPrintsEverythingInOrder) {
    // Arrange
    aera::SourceLocation loc{"test.aera", 1, 5 };

    // Act
    reporter->add_error(loc.filepath, 0, loc, "first error");
    reporter->add_warning(loc.filepath, 0, loc, "first warning");
    reporter->note(loc.filepath, 0, loc, "first note");
    reporter->add_error(loc.filepath, 0, loc, "second error");
    capture.clear();
    reporter->print_all();
    std::string output = capture.get_output();

    // Assert -> all messages present
    EXPECT_NE(output.find("first error"), std::string::npos);
    EXPECT_NE(output.find("first warning"), std::string::npos);
    EXPECT_NE(output.find("first note"), std::string::npos);
    EXPECT_NE(output.find("second error"), std::string::npos);

    // Assert -> check order (first error should come before first warning, etc.)
    size_t first_error_pos = output.find("first error");
    size_t first_warning_pos = output.find("first warning");
    size_t first_note_pos = output.find("first note");
    size_t second_error_pos = output.find("second error");

    EXPECT_LT(first_error_pos, first_warning_pos);
    EXPECT_LT(first_warning_pos, first_note_pos);
    EXPECT_LT(first_note_pos, second_error_pos);
}

// Edge Case Tests

TEST_F(DiagnosticReporterTest, EmptyMessages) {
    // Arrange
    aera::SourceLocation loc{"test.aera", 1, 5 };
    
    // Act
    reporter->add_error(loc.filepath, 0, loc, "");
    capture.clear();
    reporter->print_errors();
    std::string output = capture.get_output();

    // Assert
    EXPECT_EQ(output.find("test.aera"), std::string::npos);
    EXPECT_EQ(output.find("1:5"), std::string::npos);
}