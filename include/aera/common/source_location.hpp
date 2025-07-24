#pragma once

#include <string>

namespace aera {

    struct SourceLocation {
        std::string filepath;
        int line = 1;
        int col = 1;

        bool operator==(const SourceLocation& other) const {
            return filepath == other.filepath &&
                line == other.line &&
                col == other.col;
        }
    };
}