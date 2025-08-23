#pragma once

#include <aera/source_location.hpp>
#include <string>
#include <utility>
#include <vector>

namespace aera {

    struct Diagnostic {
        enum class Severity { Error, Warning, Note };
        Severity severity;
        std::string filepath;
        int token_length;
        SourceLocation loc;      // line and column
        std::string message;     // the message
        std::string source_line; // the source line (can be empty)
        std::string note;        // optional note (can be empty)
    };

    class DiagnosticReporter {
    public:
        void add_error(const std::string& filepath, int token_length, const SourceLocation& loc, const std::string& msg, 
            const std::string& source_line = "", const std::string& note = "");
        void add_warning(const std::string& filepath, int token_length, const SourceLocation& loc, const std::string& msg,
            const std::string& source_line = "", const std::string& note = "");
        void note(const std::string& filepath, int token_length, const SourceLocation& loc, const std::string& msg,
            const std::string& source_line = "", const std::string& note = "");

        bool has_errors() const;
        bool has_warnings() const;
        size_t error_count() const;
        size_t warning_count() const;

        void print_all();
        void print_errors();
        void print_warnings();
        void print_notes();

    private:
        std::vector<Diagnostic> diags;
        std::string severity_to_string(Diagnostic::Severity sev);
        void print_diagnostic(const Diagnostic& d);
    };
}