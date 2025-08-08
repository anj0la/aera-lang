#pragma once

#include <cstdint>
#include <variant>

namespace aera {

    struct Type {
        virtual ~Type() = default;
    };

    enum class PrimitiveTypeKind {
        Int8, Int16, Int32, Int64,
        Uint8, Uint16, Uint32, Uint64,
        Float32, Float64,
        String, Character, Bool,
    };

    struct PrimitiveType : public Type {
        PrimitiveTypeKind kind;
    };

    struct UserType : public Type {
        std::string name;
    };

    struct GenericType : public Type {
        std::string name;
        std::vector<std::unique_ptr<Type>> type_arguments;
    };

    struct ArrayType : public Type {
        std::unique_ptr<Type> element_type;
        std::vector<int> dimensions; // -1 for unsized dimensions like []
    };
}