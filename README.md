# Aera Programming Language

A statically-typed, safe and simple programming language designed as an evolutionary step forward from C++.

## Overview

Aera preserves the control and performance that C++ developers love while introducing compile-time safety and clean, simple syntax. Think of it as what C++ could have been if designed from scratch with modern safety principles in mind.

## Design Principles

Aera is built around three fundamental principles:

- **Safety** - Compile-time safety with no undefined behavior in the core language subset
- **Simplicity** - Clean syntax without complex legacy constructs like templates or multiple inheritance  
- **Performance** - No garbage collector, giving you deterministic control over memory and stack allocation

## Key Features

### Safe by Construction
- Defined core language subset with no undefined behavior
- Lifetime rules enforced at compile time
- Explicit unsafe blocks when needed for C++ interop

### C++ Compatibility  
- Call existing C++ code from Aera
- Gradual migration path from C++ projects
- Mix safe Aera code with unsafe C++ as needed

### Performance First
- Zero-cost abstractions
- No garbage collection overhead
- Direct control over memory layout and allocation

### Minimalist & Learnable
- Dropped complex C++ features that cause confusion
- No implicit conversions
- Straightforward syntax that's easy to reason about

## Language Highlights

### Modern Syntax
```aera
// Variables are immutable by default
let name = "Aera";
let mut counter = 0;

// Type annotations are optional when type can be inferred
let pi: float32 = 3.14;
const MAX_SIZE = 1000;

// Functions with clear parameter and return types
fn add(a: int32, b: int32) -> int32 {
    return a + b;
}
```

### Safety with Escape Hatches
```aera
// Safe code by default
let data = process_safely();

// Explicit unsafe blocks when needed
let result = unsafe {
    call_legacy_cpp_function(data)
};
```

### Simple Control Flow
```aera
// Familiar control structures
if condition {
    do_something();
} else {
    do_something_else();
}

while running {
    update();
    if should_exit {
        break;
    }
}
```

## Type System

Aera features a static type system with:

- **Primitive types**: `int8`, `int16`, `int32`, `int64`, `float32`, `float64`, `bool`, `char`, `string`
- **Compound types**: Arrays `[T]` and Tuples `(T1, T2, ...)`
- **Type inference**: Let the compiler figure out types when obvious
- **No implicit conversions**: Explicit is better than implicit

## Version 0.1.0 Roadmap

The initial release focuses on core functionality:

- Lexer and Parser for Aera syntax
- Transpiler to generate equivalent C++ code  
- Basic syntax: variables, literals, operators, expressions
- Control flow: if statements, while loops
- Functions: declarations, calls, parameters
- Primitive data types and basic operations
- Terminal I/O library (`print`, `println`)

## Philosophy

Aera follows the same evolutionary approach as C3's modernization of C, but for C++. Rather than breaking compatibility entirely, Aera provides a migration path that lets you:

1. Start with unsafe C++ code that works
2. Gradually refactor to safe Aera constructs
3. Keep unsafe blocks only where absolutely necessary
4. Maintain the performance characteristics you expect

---

*Aera is currently in early development. The language specification and implementation are subject to change as I refine the design based on real-world usage and feedback.*