# Error Handling
Aera provides two types for error handling: `opt!<T>` and `res!<T>`, and convenient operators when working with these types.

## Table of Contents

- [Optional Type](#optional-type)
- [Result Type](#result-type)
- [Operators](#operators)

## Optional Type

An optional type explicitly represents values that might not exist. This is the only way to represent "null" or missing values in Aera, making null pointer errors impossible for managed types.

```aera
let maybe_value: opt!<int32> = some(42)

match maybe_value { 
    some(value) => print("Got: {}", value)
    none => print("No value")
}
```

## Result Type

A result type represents operations that can either succeed with a value of type `T` or fail with an error of type `E`. This makes error handling explicit and prevents forgotten error checks.

```aera
fn divide(a: float64, b: float64) -> res!<float64, string> { 
    if b == 0.0 { 
        return err("Division by zero")
     } 
     return ok(a / b)
} 

let result = divide(10.0, 2.0); 

match result { 
    ok(value) => print("Result: {}", value)
    err(message) => print("Error: {}", message)
}
```

## Operators

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