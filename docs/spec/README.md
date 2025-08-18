# Aera Language Specification

Welcome to the **Aera Language Specification**! This folder documents the syntax, semantics, and design principles of Aera, a statically-typed, safe, and 
expressive programming language designed for interactive applications.

## Table of Contents

### 1. Overview
- [Overview](./overview.md) → Introduction to Aera, its purpose, and core principles.
  - [Performance](./overview.md#performance)
  - [Flexibility](./overview.md#flexibility)
  - [Usability](./overview.md#usability)
  - [Control](./overview.md#control)

### 2. Lexical Structure
- [Lexical Structure](./lexical.md) → Defines character set, identifiers, keywords, tokens, comments, and whitespace.
  - [Character Set](./lexical.md#character-set)
  - [Identifiers](./lexical.md#identifiers)
  - [Keywords](./lexical.md#keywords)
  - [Symbolic Tokens](./lexical.md#symbolic-tokens)
    - [Operator Tokens](./lexical.md#operator-tokens)
    - [Punctuation & Separator Tokens](./lexical.md#punctuation--separator-tokens)
    - [Other Tokens](./lexical.md#other-tokens)
  - [Comments](./lexical.md#comments)
  - [Whitespace](./lexical.md#whitespace)

### 3. Variables
- [Variables](./variables.md) → How to declare, initialize, and use variables.
  - [Declaration and Initialization](./variables.md#declaration-and-initialization)
  - [Scope](./Variables.md#scope)
    - [Shadowing](./Variables.md#shadowing)
    - [Lifetime](./Variables.md#lifetime)
  - [Naming Conventions](./Variables.md#naming-conventions)
  - [Initialization](./Variables.md#initialization)
  - [Constants](./Variables.md#constants)
  - [Type Annotations](./Variables.md#type-annotations)

### 4. Control Flow
- [Control Flow](./control_flow.md) → Conditional execution, loops, and pattern matching.
  - [if statement](./control_flow.md#if-statement)
  - [while statement](./control_flow.md#while-statement)
  - [for statement](./control_flow.md#for-statement)
  - [loop statement](./control_flow.md#loop-statement)
  - [match statement](./control_flow.md#match-statement)
  - [break](./control_flow.md#break)
  - [continue](./control_flow.md#continue)
  - [return](./control_flow.md#return)

### 5. Types
- [Types](./types.md) → Overview of primitive, compound, memory management, and error handling types.
  - [Primitive Types](./types.md#primitive-types)
    - [Integer](./types.md#integer)
    - [Floating Point](./types.md#floating-point)
    - [Boolean](./types.md#boolean)
    - [Character](./types.md#character)
    - [String](./types.md#string)
  - [Compound Types](./types.md#compound-types)
    - [Array](./types.md#array)
    - [Tuple](./types.md#tuple)
    - [Map](./types.md#map)
  - [Memory Management](./types.md#memory-management)
    - [Owned Pointer](./types.md#owned-pointer)
    - [Shared Reference](./types.md#shared-reference)
    - [Weak Reference](./types.md#weak-reference)
    - [Raw Pointer](./types.md#raw-pointer)
  - [Error Handling](./types.md#error-handling)
    - [Optional Type](./types.md#optional-type)
    - [Result Type](./types.md#result-type)
    - [Operators](./types.md#operators)

### 6. Grammar
- [Grammar](./grammar.md) → Formal grammar rules using EBNF notation.
  - [Declarations](./grammar.md#declarations)
    - [Function Declaration](./grammar.md#function-declaration)
    - [Variable Declaration](./grammar.md#variable-declaration)
    - [Type Declaration](./grammar.md#type-declaration)
  - [Expressions](./grammar.md#expressions)
    - [Precedence and Associativity](./grammar.md#precedence-and-associativity)
  - [Statements](./grammar.md#statements)
  - [Lexical Grammar](./grammar.md#lexical-grammar)
