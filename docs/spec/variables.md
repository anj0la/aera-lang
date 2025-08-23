# Variables

Variables in Aera are named values that can be immutable or mutable, with optional type declarations.

## Table of Contents

- [Declaration and Initialization](#declaration-and-initialization)
- [Scope](#scope)
  - [Shadowing](#shadowing)
  - [Lifetime](#lifetime)
- [Naming Conventions](#naming-conventions)
- [Initialization](#initialization)
- [Constants](#constants)
- [Type Annotations](#type-annotations)

## Declaration and Initialization

Variables are declared in Aera using the `let` keyword. 

```aera
let num: int32 = 5 # immutable, CANNOT be changed once assigned
num = 4 # NOT allowed
```

Variables are immutable by default in Aera.This means that once a variable has been assigned, the value stored at its memory location cannot be changed.
To make a variable mutable, the `mut` keyword must be used after the `let` keyword.

```aera
let mut num: int32 = 5 # mutable, CAN be changed once assigned
num = 4 # NOW allowed
```

## Scope

Aera uses lexical (block) scope. A variable is only accessible within the block ({ … }) in which it is declared.
Global variables are defined outside of a block and are accessible in all functions in an .aera file, regardless o
f whether a function is private or public.

### Shadowing

A local variable can shadow a variable from an outer scope:

```aera
let x: int32 = 5
{
    let x: int32 = 10  # shadows outer x
    println(x)         # prints 10
}
println(x)             # prints 5
```

### Lifetime

- Variables are typically created when their block is entered and destroyed when the block exits.
- Global variables exist for the duration of the program.

## Naming Conventions

Variables in Aera must start with a letter (`a–z` or `A–Z`) or underscore (`_`), followed by any alphanumeric character or underscore.

The reserved words in Aera are:
`fn, let, mut, const, pub, modifies, if, else, for, while, loop, match, break, continue, return, import, class, struct, enum, trait, with, alias, self, as, bind`

These words **cannot be used as variable names** unless escaped with the grave accent (`).

```aera
let `fn` = 5       # valid
let fn = 5         # invalid
```

Variable names are recommended to use `snake_case` to follow Aera’s style.

## Initialization

In Aera, a variable may or may not be initialized at the time of declaration:

```aera
let x: int32             # valid, type declared but no initial value
let y = 0                # valid, type inferred from expression
let z: float64 = 3.14    # valid, explicit type and initialization
```

- If a variable is **declared without an initializer**, its value is considered **uninitialized**.
- Using an uninitialized variable before assigning a value will result in a **compilation error**.

## Constants

Constants differ from variables; they **cannot be changed or made mutable**. Use the `const` keyword to declare a constant:

```aera
const pi: float64 = 3.14
```

## Type Annotations

Variables can have explicit types or rely on type inference.

```aera
let num: int32 = 5   # explicit type
let num2 = 10        # type inferred
```
