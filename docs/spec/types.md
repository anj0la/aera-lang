# Types
Aera’s type system is designed to be expressive, safe, and easy to understand. The available types fall into four categories: primitive, compound, memory management, and error handling types.

## Primitive Types

### Integer
Aera provides signed and unsigned integer types, similar to C++. If no type is specified, integers default to int32. Unsigned types are prefixed with u. The built-in integer types are:
- `int8` / `uint8`
- `int16` / `uint16`
- `int32` / `uint32`
- `int64` / `uint64`
- `intptr` / `uintptr`

Aera also supports binary, octal, and hexadecimal notation, using the prefixes `0b`, `0o`, and `0x` (uppercase variants are also supported).

### Floating Point

Aera provides two floating-point types: `float32` and `float64`.

- `float32` corresponds to C++’s float.
- `float64` corresponds to C++’s double.

Scientific notation is supported.

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
let mut map: map!<str, int32> = {"apple": 0, "banana": 1, "orange": 2 }
```

## Memory Management

Aera provides three pointer types for managing memory safely without garbage collection: `ptr!<T>`, `ref!<T>` and `weak!<T>`.

### Owned Pointer

An owned pointer provides exclusive ownership of heap-allocated memory. It automatically deallocates memory when the pointer goes out of scope. It works similarly to std::unique_ptr<T> in C++.

```aera
let a = ptr!<MyStruct>(...)  # a owns the value
```

Transferring ownership is done using the move function. Once a pointer has been moved, the original pointer is no longer valid, ensuring that no two active pointers own the same object at the same time.

```aera
let a = ptr!<MyStruct>(...)  # a owns the value
let b = a.move() # transfers ownership to b
```

### Shared Reference

A shared reference allows multiple owners of the same memory location through reference counting. The memory is deallocated when the last reference is dropped. It works similarly to std::shared_ptr<T> in C++.

```aera
let a = ref!<MyStruct>(...) # shared ownership
let b = a.clone() # ref count increases (copy)
```

### Weak Reference

A weak reference holds a non-owning reference to memory managed by ref!<T>. It does not affect the reference count and can detect when the referenced object has been deallocated.

```aera
let a = ref!<MyStruct>(...) # shared ownership
let w = a.downgrade() # where a is a ref!<T>
let u = w.upgrade() # attempts to get strong ref
```

### Raw Pointer

Raw pointers (`*T`) are not available in the safe domain. They are only accessible via a special function T* to_raw(...) inside bind blocks.

```aera
let obj = ptr!<MyStruct>(...)

bind “cpp” {
    MyStruct* raw_ptr = to_raw(obj); # raw_ptr is a MyStruct*
    raw_ptr->do_cpp_logic()
}
```

This ensures that all pointer-based memory access in safe code is mediated by smart pointer types, eliminating common memory errors. 

## Error Handling

Aera provides two types for error handling: `opt!<T>` and `res!<T>`.

### Optional Type

An optional type explicitly represents values that might not exist. This is the only way to represent "null" or missing values in Aera, making null pointer errors impossible for managed types.
```aera
let maybe_value: opt!<int32> = some(42)

match maybe_value { 
    some(value) => print("Got: {}", value), 
    none => print("No value"), 
}
```

### Result Type

A result type represents operations that can either succeed with a value of type T or fail with an error of type E. This makes error handling explicit and prevents forgotten error checks.
```aera
fn divide(a: float64, b: float64) -> res!<float64, string> { 
    if b == 0.0 { 
        return err("Division by zero")
     } 
     return ok(a / b)
} 

let result = divide(10.0, 2.0); 

match result { 
    ok(value) => print("Result: {}", value), 
    err(message) => print("Error: {}", message), 
}
```

### Operators

Aera provides two convenient operators for working with errors: `?` and `??`.

The `?` operator is used to unwrap optional values. When applied to an expression that may be `some` or `none`, it unwraps the contained value if present, or returns `none` immediately from the current function if the value is absent.

```aera
fn get_length(s: opt!<string>) -> opt!<int> {
    let str = s? # unwrap option, else return none from get_length
    return str.length()
}

fn main() {
    let maybe_name: opt!<string> = some("Julia")
    let length = get_length(maybe_name)?  # unwrapping option here as well
    println(length)
}
```

The `??` operator is used to unwrap error values. It attempts to unwrap the `ok` value from a result. If the result contains an `err`, `??` returns the error immediately from the current function, propagating the failure without requiring explicit error handling.

```aera
fn read_file(path: string) -> res!<string, err> {
    let content = open_file(path)??  # unwrap ok or return err early
    return ok(content)
}

fn process() -> res!<void, err> {
    let data = read_file("data.txt")??  # propagate err early if any
    println(data)
    return ok()
}
```

The `!!` operator is used to provide a fallback value for both optional and result types. When applied to an expression that may be none (for opt!) or err (for res!), it unwraps the contained value if present, or returns the specified fallback value instead. This allows you to handle missing or failing values concisely without propagating them further.

```aera
# Using !! with an optional
let maybe_name: opt!<string> = none
let display_name = maybe_name !! "Guest" # use "Guest" if maybe_name is none
println(display_name)

# Using !! with a result
fn load_logo(path: string) -> res!<Image, err> { 
    return err("file not found") # assume this might fail
}

let logo = load_logo("logo.png") !! placeholder_image # use placeholder_image if load fails
render(logo)

```




