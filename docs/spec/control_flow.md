# Control Flow

Aera supports familiar control flow structures such as `if`, `while`,`for` and `match`.

## Table of Contents

- [if expression](#if-expression)
- [while expression](#while-expression)
- [for expression](#for-expression)
- [loop expression](#loop-expression)
- [match expression](#match-expression)
- [break](#break)
- [continue](#continue)
- [return](#return)

## if expression

the following block executes. Otherwise, the code in the else block is executed.

The `if` expression is structured similarly to other languages, where the condition after the keyword `if` and before the left brace `{` must evaluate to a bool. The difference however, is that each block must return a value. 

If the condition evaulates to true, then the following block is executed and returns a value. Otherwise, the code in the else block is executed and also returns a value.

```aera
if cond {
    do_something() # returns a value
}
else {
    do_something_else() # returns a value
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

Aera does not provide a ternary operator, however, you can "mimic" the presence of one with the following line of code:

```aera
let result = if cond { do_something } else { do_something_else() }
```

## while expression

The `while` expression is the first type of loop that Aera provides The condition is evaluated before each iteration. As long as the condition is true, the loop continues.

```aera
while cond {
    do_something()
}
```

Unlike other languages, Aera does not provide a do-while loop, where the loop is executed at least once. The equivalent of a do-while loop in Aera would look like the following:

```aera
if true { # always will run 
    do_something();
}

while cond { # may not run if cond == false
    do_something()
}
```

## for expression

Aera's `for` loop iterates directly over ranges or collections, which eliminates the need for complex loop syntax.

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

## loop expression

The `loop` expression creates an infinite loop. 

```aera
loop {
    do_something_forever()
}
```

It is equivalent to creating a while loop with a condition that is always true:

```aera
while true {
    do_something_forever()
}
```

## match expression

The `match` expression compares a value against a series of patterns, executing the code associated with the first matching pattern. Patterns starting with an underscore (_) are wildcards, matching any value without binding it.

```aera
let val = 2

match val { 
    1 => print("1")
    2 => print("2")
    _ => print("Any")
}
```

## break

The `break` expression stops execution inside of a loop.

```aera
while cond {
    do_something()

    if cond {
       break # break out of the while loop
     }
}
```

## continue 

The `continue` expression immediately terminates the current iteration of a loop and proceeds to the next iteration. 

```aera
while cond {
    do_something()

    if cond {
       continue # skip current iteration
     }
}
```

## return

The `return` expression exits a function and returns a value.

```aera
for i in 0..10 {
    if cond {
      return true
    }
}

return false
```


