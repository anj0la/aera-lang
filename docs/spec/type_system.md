# Type System
Aera’s type system is designed to be expressive, safe, and simple. The available types fall into two categories: primitive and compound types.

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
let fruits: map!<str, int32> = {"apple": 0, "banana": 1, "orange": 2 }
```