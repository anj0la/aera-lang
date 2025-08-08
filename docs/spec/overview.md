# Overview

Aera is a statically-typed, safe and simple programming language designed as an evolutionary step forward from C++, preserving control and performance while introducing ad-hoc polymorphism, compile-time safety and clean syntax. Its goal is to be a language suitable for developing interactive applications, such as games and graphical user interfaces (GUIs). Aera operates on four core principles that guide its design: Performance, Flexibility, Useability and Control.

## Core Principles

### Performance
Aera is a compiled language, with a **source-to-source** compiler, also known as a **transpiler**. Aera converts its source code to a more stable and lower-level language, C++. Targeting C++ code allows Aera to leverage mature compiler backends (e.g., GCC, Clang), making it easier to integrate with existing projects and platforms.

Memory in Aera is managed through ownership, value semantics, and explicit lifetimes. This allows Aera to be performant without a garbage collector, which eliminates runtime pauses and delivers predictable behavior.

### Flexibility
Aera's trait system enables **compositional** programming without enforcing inheritance. Structs are for data, while Classes support inheritance when needed. Traits define reusable behavior and can be applied to both. This avoids the "everything inherits from Object" problem while still giving programmers polymorphism and modularity.

Aera’s transpiler approach makes C++ interoperability trivial and C interoperability easy by extension, making it suitable for embedding in legacy engines or gradual adoption in existing systems. 

### Usability
Aera is designed with usability in mind, offering a readable and expressive syntax without compromising control. Its clean, minimal structure draws inspiration from high-level languages like Python and Go. Error handling is intuitive and explicit, thanks to the *?* and *??* operators, while its default behaviors are deliberately safe and predictable.

### Control
Aera’s memory management is **explicit**. It provides safe pointer types to help manage memory without garbage collection. Unsafe operations and raw pointer manipulations are only allowed inside explicit bind blocks. These blocks create clear, compiler-checked boundaries between safe and unsafe code, to maintain memory safety without sacrificing control.

