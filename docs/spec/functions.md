# Functions

This page provides a high-level overview of functions in Aera. Functions are (or will be) first class in Aera, meaning they can be used like any other value.

> This page is still in development.

## Table of Contents

- [Syntax](#syntax)
- [Main](#Main)

## Syntax

Functions are defined using the `fn` keyword. 

```aera
# function syntax
fn function_name(param1: type, param2: type) -> return_type {
    function block
}

# example of a simple function
fn count(n: int32) -> () { # unit type returns nothing
    for i in 0..n {
        println(i)
    }
}
```

## Main

Aera provides the `main` function which represents the entry point for code execution.

```aera
fn main() {
    println("Hello world!") # implictly returns the unit type ()
}
```