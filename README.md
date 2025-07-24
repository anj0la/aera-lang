# Aera Programming Language

A statically-typed, safe and simple programming language designed as an evolutionary step forward from C++.

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