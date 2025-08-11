#pragma once

#include <vector>
#include <string>
#include <sstream>
#include <string_view>

namespace aera {
    class SourceContext {
    public:
        SourceContext(const std::string& filename, const std::string& source) : filename_(filename), source_(source) {
            size_t start = 0;
            for (size_t i = 0; i < source_.size(); ++i) {
                if (source_[i] == '\n') {
                    line_spans_.emplace_back(start, i - start);
                    start = i + 1;
                }
            }

            if (start < source_.size()) {
                line_spans_.push_back({ start, source_.size() - start });
            }
        }

        std::string_view get_line(size_t line) const {
            auto [start, length] = line_spans_[line];
            return std::string_view(source_).substr(start, length);
        }

        const std::string& source() const { return source_; }
        const std::string& filename() const { return filename_; }
    private:
        struct LineSpan{ size_t start; size_t length; }; // constructor automatically made
        std::string source_;
        std::string filename_;
        std::vector<LineSpan> line_spans_;

    
    };
}