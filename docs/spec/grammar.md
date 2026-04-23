# Grammar

Aera's grammar is used to parse the sequence of tokens into the abstract syntax tree. It starts off with the first rule which matches an entire Aera program.

## Table of Contents

- [Program](#declarations)
- [Expressions](#expressions)
  - [Precedence and Associativity](#precedence-and-associativity)
  - [Two Types of Expressions](#two-types-of-expressions)
  - [Expression With Block](#expression-with-block)
  - [Expression Without Block](#expression-without-block)
- [Statements](#statements)

## Program
A program is a sequence of items, top-level components that make up the module.

## Expressions
Expressions compute values in Aera. There are a number of unary and binary operators with different levels of precedence.

### Precedence and Associativity
The following table showcases the operator precedence, along with its associativity rules.

| Precedence | Operators | Description | Associativity |
|------------|-----------|-------------|---------------|
| 1 (Highest) | (),  ?, ?? | Function call, Try Propagation, Fallback | Left-to-right |
| 2 | !, ~ |  Unary NOT, Bitwise NOT | Right-to-left |
| 3 | *, /, % | Multiplication, Division, Modulus | Left-to-right |
| 4 | +, - | Addition, Subtraction | Left-to-right |
| 5 | <<, >> | Bitwise shifts | Left-to-right |
| 6 | <, <=, >, >= | Relational operators | Left-to-right |
| 7 | & | Bitwise AND | Left-to-right |
| 8 | ^ | Bitwise XOR | Left-to-right |
| 9 | \| | Bitwise OR | Left-to-right |
| 10 | && | Logical AND | Left-to-right |
| 11 | \|\| | Logical OR | Left-to-right |
| 12 (Lowest) | =, +=, -=, *=, /=, %=, <<=, >>=, &=, \|=, ^- | Assignment and compound assignment | Right-to-left |

### Two Types of Expressions

Aera splits expressions into two types: ones containing block expressions and ones without it. 

The former includes expressions usually represented as statements in other languages, such as the if, while and for loop expressions.

The latter includes expressions that are expected in programming languages (e.g., a + b).

### Expression With Block

Listed below are the grammar rules for expressions containing block expressions. 

These are expressions that produce a value inside of a block. In other languages, these expressions are "statements" and do not return a value. However, in Aera, these expressions do return a value. Examples include:

```aera
loop {
    do_something_forever() # returns the unit type
}

if cond {
    return 1
}
else {
    return 2
}
```

### Expression Without Block

These are expressions that are seen in other languages. Examples include:

```aera
1 + 2 + 3
4 / 5
12 == 12
2 && 5
```

## Statements

Statements introduce or control structure in Aera, as opposed to statements in other familiar languages.
Let statements are immutable, and mutability will be added once the language further develops.

```aera
let name: string = "Jane" # let statement
const pi: float32 = 3.14  # const statement
1 + 2                     # expression statement
```