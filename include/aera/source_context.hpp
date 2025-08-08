#pragma once

#include <vector>
#include <string>
#include <sstream>

namespace aera {
    class SourceContext {
    public:
        SourceContext(const std::string& filename, const std::string& source) : filename_(filename), source_(source) {
            std::istringstream stream(source);
            std::string line;
            while (std::getline(stream, line)) {
                lines_.push_back(line);
            }
        }

        const std::string& source() const { return source_; }
        const std::string& get_line(size_t line) const { return lines_[line]; }
        const std::string& filename() const { return filename_; }
    private:
        std::string source_;
        std::vector<std::string> lines_;
        std::string filename_;
    };
}