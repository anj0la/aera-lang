#pragma once

#include <cstdint>
#include <variant>
#include <vector>
#include <string>
#include <memory>

namespace aera {

    // Forward Declarations

    struct Type;
    struct PrimitiveType;
    struct UserType;
    struct GenericType;
    struct DynamicArrayType;
    struct MapType;
    struct SetType;
    struct StaticArrayType;

    // Types

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

    struct GenericType : public UserType {
        std::vector<std::unique_ptr<Type>> type_arguments;
    };

    struct DynamicArrayType : public Type {
        std::unique_ptr<Type> element_type;
    };

    struct MapType : public Type {
        std::unique_ptr<Type> key_type;
        std::unique_ptr<Type> value_type;
    };

    struct SetType : public Type {
        std::unique_ptr<Type> element_type;
    };

    struct StaticArrayType : public Type {
        std::unique_ptr<Type> element_type;
        std::vector<int> dimensions;
    };
}