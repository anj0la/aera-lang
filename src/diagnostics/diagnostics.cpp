#include <aera/diagnostics/diagnostics.hpp>
#include <algorithm>
#include <iostream>

namespace aera {

    void DiagnosticReporter::add_error(const std::string& filepath, int token_length, const SourceLocation& loc, const std::string& msg,
        const std::string& source_line = "", const std::string& note = "") {
        diags.emplace_back(Diagnostic{ Diagnostic::Severity::Error, filepath, token_length, loc, msg, source_line, note });
    }

    void DiagnosticReporter::add_warning(const std::string& filepath, int token_length, const SourceLocation& loc, const std::string& msg,
        const std::string& source_line = "", const std::string& note = "") {
        diags.emplace_back(Diagnostic{ Diagnostic::Severity::Warning, filepath, token_length, loc, msg, source_line, note });
    }
    void DiagnosticReporter::note(const std::string& filepath, int token_length, const SourceLocation& loc, const std::string& msg,
        const std::string& source_line = "", const std::string& note = "") {
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
            print_diagnostic(d); // no need to switch, don't matter

            switch (d.severity) {
                case Diagnostic::Severity::Error:
                    print_error(d);
                    break;
                case Diagnostic::Severity::Warning:
                    print_warning(d);
                    break;
                case Diagnostic::Severity::Note:
                    print_note(d);
                    break;
            }
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

    std::string severity_to_string(Diagnostic::Severity sev) {
        switch (sev) {
            case Diagnostic::Severity::Error:
                return "error";
            case Diagnostic::Severity::Warning:
                return "warning";
            case Diagnostic::Severity::Note:
                return "note";
        }
             
    }

    void DiagnosticReporter::print_diagnostic(const Diagnostic& d) {
        // First line is the same for EVERY type of diagnostic: filepath:line:col: severity: message
        std::cerr << d.filepath << ":" << d.loc.line << ":" << d.loc.col
            << ": " << severity_to_string() << ": " << d.msg << std::endl;

        // Second line -> source line, only valid for ERROR / WARNINGS
        if (d.severity == Diagnostic::Severity::Error || Diagnostic::Severity::Warning) {
            std::cerr << "    " << d.source_line << std::endl;
        }

        // Third line -> caret and tilde symbols to highlight WHERE the error / warning is
        std::cerr << std::string(d.loc.col - 1, ' ') << "^";

        // If not EOF, then add tildes to highlight error / warning
        if (d.token_length > 0) {
            for (size_t = 1; i < d.token_length; ++i) {
                std::cerr << "~";
            }
        }
     
        std::cerr << std::endl;

        // Last line -> optional note for more information
        if (!d.note.empty()) {
            std::cerr << "    " << "note: " << d.note << std::endl;
        }
    }
}


main.aera:7:16: error :unexpected ','
    fn(first, second, )
              ^~~~~~~~
    note: remove the comma after the last argument