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
- [Lexical Grammar](#lexical-grammar)

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

## Lexical Grammar

```ebnf
literal = integer_literal
          | float_literal
          | character_literal
          | string_literal
          | boolean_literal ;

integer_literal = decimal_integer
                | hexadecimal_integer
                | binary_integer
                | octal_integer ;

decimal_integer = digit { digit } ;
hexadecimal_integer = "0" ( "x" | "X")  hex_digit { hex_digit } ;
binary_integer = "0" ( "b" | "B" ) binary_digit { binary_digit } ;
octal_integer = "0" ( "o" | "O" ) octal_digit { octal_digit } ;
float_literal = decimal_integer "." decimal_integer [ scientific_notation ]
              | decimal_integer scientific_notation ;
scientific_notation = ( "e" | "E" ) [ "+" | "-" ] decimal_integer ;

float_literal = integer_literal "." integer_literal ;
character_literal = "'" character  "'" ";" ;
string_literal = """ { character } """ ";" ;
boolean_literal = "true" | "false" ;

character = printable | escape_sequence ;
printable = alpha | digit | symbol | space ;
alpha = "a" ... "z" | "A" ... "Z" ;
digit = "0" ... "9";
hex_digit = digit | "a" ... "f" | "A" ... "F" ;
binary_digit = "0" | "1" ;
octal_digit = "0" ... "7" ;
symbol = "!" ... "/" | ":" ... "@" | "[" ... "`" | "{" ... "~" ;
underscore = "_" ;
escape_sequence = "\\" ( "n" | "t" | "r" | "\"" | "'" | "\\" ) ;
space = " " ;

identifier = ( alpha | "_" ) { ( alpha | digit | "_" ) } ;

type = primitive_type ; 

primitive_type = "int8" | "int16" | "int32" | "int64" | "uint8" | "uint16" | "uint32" | "uint64" | "float32" | "float64" | "char" | "string" | "bool" ; 
```
