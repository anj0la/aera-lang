# Type System
Aera’s type system is designed to be expressive, safe, and simple. Many of the types listed below are types seen in other languages, including variant, 
option and result types seen in many popular functional programming languages, such as OCaml.


> In Aera, types are immutable by default. To make them mutable, the `mut` keyword must be provided after the `let` keyword.

## Table of Contents

- [Primitive Types](#primitive-types)
  - [Integer](#integer)
  - [Floating Point](#floating-point)
  - [Boolean](#boolean)
  - [Character](#character)
  - [String](#string)
- [Compound Types](#compound-types)
  - [Array](#array)
  - [Tuple](#tuple)
  - [Map](#map)
- [Option and Result Types](#option-and-result-types)
  - [Option](#option)
  - [Result](#result)
  - [Operators](#operators)
- [User Defined Types](#user-defined-types)
  - [Variant](#variant)
  - [Struct](#struct)


## Primitive Types

### Integer
Aera provides signed and unsigned integer types. If no type is specified, integers default to int32. Unsigned types are prefixed with u. The built-in integer types are:
- `int8` / `uint8`
- `int16` / `uint16`
- `int32` / `uint32`
- `int64` / `uint64`
- `intptr` / `uintptr`

Aera also supports binary, octal, and hexadecimal notation, using the prefixes `0b`, `0o`, and `0x` (uppercase variants are also supported).

### Floating Point

Aera provides two floating-point types: `float32` and `float64`.

### Boolean

A `bool` is an integer type that can be either `true` or `false`, occupying 1 byte of memory.

### Character

A `char` is an integer type that stores a single character and occupies 1 byte of memory. Characters are enclosed in single quotation marks ('').

### String

A `string` is a **sequence** of characters, enclosed in double quotation marks ("").

## Compound Types

### Array

An array is a fixed-size data structure that stores elements of the same type (homogeneous) in contiguous memory.

Aera provides two kinds of arrays:
- Static arrays → Declared by specifying the element type. The size is **fixed** at declaration; elements can change, but the length cannot.
- Dynamic arrays → Declared using the `arr!<T>` container. They support adding and removing elements at runtime. Unless the array size is known in advance, dynamic arrays are **preferred**.

```aera
let whole_numbers: int32[10] = [0, 1, 2, 3, 4, 5, 6, 7, 8, 9]
let arr: arr!<float64> = [1.0, 2.0, 4.0, 8.0]
```

### Tuple

A tuple is a fixed-size data structure that stores elements of different types (heterogeneous) in contiguous memory.
Tuples are useful for returning multiple values from a function.

```aera
let tuple: (string, bool) = ("hello world", true)
```

### Map

A `map!<K, V>` is a generic associative container that stores key-value pairs, where each key of type `K` is uniquely associated with a value of type `V`. It provides efficient lookup, insertion, and removal operations based on keys.

- `K` represents the type of the keys.
- `V` represents the type of the values.

Maps are useful for storing and retrieving data by keys quickly and are implemented using a hash-based structure under the hood.

```aera
let fruits: map!<str, int32> = {"apple": 0, "banana": 1 }
```

## Option And Result Types

### Option

An option type explicitly represents values that might not exist. This is the only way to represent "null" or missing values in Aera, making null pointer errors impossible for managed types.

```aera
let maybe_value: opt!<int32> = some(42)

match maybe_value { 
    some(value) => print("Got: {}", value)
    none => print("No value")
}
```

### Result

A result type represents operations that can either succeed with a value of type `T` or fail with an error of type `E`. This makes error handling explicit and prevents forgotten error checks.

```aera
fn divide(a: float64, b: float64) -> res!<float64, string> { 
    if b == 0.0 { 
        err("Division by zero")
    } 
    ok(a / b)
} 

let result = divide(10.0, 2.0); 

match result { 
    ok(value) => print("Ok: {}", value)
    err(message) => print("Err: {}", message)
}
```

### Operators


Aera provides two convenient operators for working with optionals and errors: `?` and  `??`.

The `?` operator is used to unwrap both optional and error values. 

When applied to an optional (`opt!<T>`), it unwraps the contained value if present, or returns `none` immediately from the current function if the value is absent. 

```aera
fn get_length(s: opt!<string>) -> opt!<int> {
    let str = s?  # unwrap option, else return none from get_length
    return str.length()
}
```

When applied to a result (`res!<T, E>`), it attempts to unwrap the `ok` value, or returns the error immediately from the current function if the result contains an `err`.

```aera
fn read_file(path: string) -> res!<string, err> {
    let content = open_file(path)?  # unwrap ok or return err early
    return ok(content)
}
```

The `??` operator is used to provide a fallback value for both optional and result types. 

When applied to an expression that may be none (for `opt!<T>`) or err (for `res!<T, E>`), it unwraps the contained value if present, or returns the specified fallback value instead. This allows one to handle missing or failing values concisely without propagating them further.

```aera
fn get_name_or_default(maybe_name: opt!<string>) -> string {
    return maybe_name ?? "Anonymous" # use "Anonymous" if none
}

fn safe_divide(a: int, b: int) -> int {
    return divide(a, b) ?? 0 # use 0 if division returns an error
}
```

## User Defined Types

### Variant

A variant is a sum type (or tagged union).

At their simplest, they look like enums from C or C++, and They are defined using the `variant` keyword:

```aera
variant Shape {
    Circle
    Triangle
    Rectangle
    Square
}
```

Variants can store data through the use of constructors. If a variant has no constructor, it stores no data. If it does, then it stores the data that is defined by the constructor.

```aera
variant Shape {
    Circle(radius: float32)
    Triangle(base: float32, height: float32)
    Rectangle(width: float32, height: float32)
    Square(side: float32)
}
```

### Struct

A struct is a product type. It defines a compositie data type.

They work similary to C / C++ structs, with the difference being that structs are immutable by default.

```aera
struct Player = {
    hp: int32
    name: string
}
```