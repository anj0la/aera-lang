#include <aera/diagnostics.hpp>
#include <algorithm>
#include <iostream>

namespace aera {

    void DiagnosticReporter::add_error(const std::string& filepath, int token_length, const SourceLocation& loc, const std::string& msg,
        const std::string& source_line, const std::string& note) {

        if (msg.empty() || std::all_of(msg.begin(), msg.end(), ::isspace)) {
            return;
        }

        diags.emplace_back(Diagnostic{ Diagnostic::Severity::Error, filepath, token_length, loc, msg, source_line, note });
    }

    void DiagnosticReporter::add_warning(const std::string& filepath, int token_length, const SourceLocation& loc, const std::string& msg,
        const std::string& source_line, const std::string& note) {

        if (msg.empty() || std::all_of(msg.begin(), msg.end(), ::isspace)) {
            return;
        }

        diags.emplace_back(Diagnostic{ Diagnostic::Severity::Warning, filepath, token_length, loc, msg, source_line, note });
    }
    void DiagnosticReporter::note(const std::string& filepath, int token_length, const SourceLocation& loc, const std::string& msg,
        const std::string& source_line, const std::string& note) {

        if (msg.empty() || std::all_of(msg.begin(), msg.end(), ::isspace)) {
            return;
        }

        diags.emplace_back(Diagnostic{ Diagnostic::Severity::Note, filepath, token_length, loc, msg, source_line, note });
    }

    bool DiagnosticReporter::has_errors() const {
        return std::any_of(diags.begin(), diags.end(),
            [](const Diagnostic& d) { return d.severity == Diagnostic::Severity::Error; });
    }

    bool DiagnosticReporter::has_warnings() const {
        return std::any_of(diags.begin(), diags.end(),
            [](const Diagnostic& d) { return d.severity == Diagnostic::Severity::Warning; });
    }

    size_t DiagnosticReporter::error_count() const {
        return std::count_if(diags.begin(), diags.end(),
            [](const Diagnostic& d) { return d.severity == Diagnostic::Severity::Error; });
    }

    size_t DiagnosticReporter::warning_count() const {
        return std::count_if(diags.begin(), diags.end(),
            [](const Diagnostic& d) { return d.severity == Diagnostic::Severity::Warning; });
    }

    void DiagnosticReporter::print_all() {
        for (const auto& d : diags) {
            print_diagnostic(d);
        }
    }

    void DiagnosticReporter::print_errors() {
        for (const auto& d : diags) {
            if (d.severity == Diagnostic::Severity::Error) {
                print_diagnostic(d);
            }
        }
    }

    void DiagnosticReporter::print_warnings() {
        for (const auto& d : diags) {
            if (d.severity == Diagnostic::Severity::Warning) {
                print_diagnostic(d);
            }
        }
    }

    void DiagnosticReporter::print_notes() {
        for (const auto& d : diags) {
            if (d.severity == Diagnostic::Severity::Note) {
                print_diagnostic(d);
            }
        }
    }

    std::string DiagnosticReporter::severity_to_string(Diagnostic::Severity sev) {
        switch (sev) {
            case Diagnostic::Severity::Error:
                return "error";
            case Diagnostic::Severity::Warning:
                return "warning";
            case Diagnostic::Severity::Note:
                return "note";
            default:
                return "unknown";
        }
             
    }

    void DiagnosticReporter::print_diagnostic(const Diagnostic& d) {
        // First line -> same for EVERY type of diagnostic: filepath:line:col: severity: message
        std::cerr << d.filepath << ":" << d.loc.line << ":" << d.loc.col
            << ": " << severity_to_string(d.severity) << ": " << d.message << std::endl;

        // Second line -> source line (if provided)
        if (!d.source_line.empty()) {
            std::cerr << "    " << d.source_line << std::endl;

            // Third line -> caret and tilde symbols to highlight the error location
            std::cerr << "    " << std::string(d.loc.col - 1, ' ') << "^";

            // Add tildes for multi-character tokens (token_length > 1)
            if (d.token_length > 1) {
                for (int i = 1; i < d.token_length; ++i) {
                    std::cerr << "~";
                }
            }
            std::cerr << std::endl;
        }

        // Optional note for more information
        if (!d.note.empty()) {
            std::cerr << "    " << "note: " << d.note << std::endl;
        }
    }
}