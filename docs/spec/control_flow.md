# Control Flow

Aera supports familiar control flow structures from C++, such as `if` and `while`, while enhancing others like `for` and `match` (an improved switch statement).

## Table of Contents

- [if statement](#if-statement)
- [while statement](#while-statement)
- [for statement](#for-statement)
- [loop statement](#loop-statement)
- [match statement](#match-statement)
- [break](#break)
- [continue](#continue)
- [return](#return)

## if statement

Aera's `if` statement structure is similar to C++, where the condition after the keyword `if` and before the left brace `{` must evaluate to a bool. If it is true, then the code in 
the following block executes. Otherwise, the code in the else block is executed.

```aera
if cond {
    do_something()
}
else {
    do_something_else()
}
```

Aera handles additional conditions with `else if`. Each `else if` is evaluated only if the preceding conditions are false.

```aera
if cond1 {
    execute_cond1_block()
}
else if cond2 {
    execute_cond2_block()
}
else {
    execute_else_block()
}
```

## while statement

The `while` statement enables looping in Aera. The condition is evaluated before each iteration. As long as the condition is true, the loop continues.

```aera
while cond {
    do_something()
}
```
## for statement

Aera’s `for` statement differs from C++ by simplifying iteration. In C++, a `for` loop typically requires an initialization variable, an end condition, and a step expression. 
In contrast, Aera’s `for` iterates directly over ranges or collections, eliminating the need for complex loop syntax. This design is closer to Python and Rust.

To iterate over a range of values using the `start..stop` syntax (where `start` is inclusive and `stop` is exclusive):

```aera
for i in 0..10 {
    do_something()
}
```

To include the `stop` value in the range, use `..=`:

```aera
for i in 0..=10 {
    do_something()
}
```

You can also iterate directly over a collection:

```aera
for value in collection {
    do_something()
}
```

## loop statement

The `loop` statement creates an infinite loop. 

```aera
loop {
    do_something_forever()
}
```

## match statement

The `match` statement compares a value against a series of patterns, executing the code associated with the first matching pattern. 
It works similarly to C++’s `switch` statement, but with a cleaner syntax. Patterns starting with an underscore (_) are wildcards, matching any value without binding it.

```aera
let val = 2

match val { 
    1 => print("1"),
    2 => print("2"),
    _ => print("Any"),
}
```

## break

The `break` statement stops execution inside of a loop.

```aera
while cond {
    do_something()

    if cond {
       break # break out of the while loop
     }
}
```

## continue 

The `continue` statement immediately terminates the current iteration of a loop and proceeds to the next iteration. 

```aera
while cond {
    do_something()

    if cond {
       continue # skip current iteration
     }
}
```

## return

The `return` statement exits a function and optionally returns a value.

```aera
for i in 0..10 {
    if cond {
      return true
    }
}

return false

```


