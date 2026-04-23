# Grammar

Aera's grammar is used to parse the sequence of tokens into the abstract syntax tree. It starts off with the first rule which matches an entire Aera program.

## Table of Contents

- [Declarations](#declarations)
  - [Function Declaration](#function-declaration)
  - [Variable Declaration](#variable-declaration)
  - [Type Declaration](#type-declaration)
- [Expressions](#expressions)
  - [Precedence and Associativity](#precedence-and-associativity)
  - [Two Types of Expressions](#two-types-of-expressions)
  - [Expression With Block](#expression-with-block)
  - [Expression Without Block](#expression-without-block)
- [Statements](#statements)
- [Lexical Grammar](#lexical-grammar)

## Program
A program is a sequence of items, top-level components that make up the module.

```ebnf
program = { item } EOF ;
```

```ebnf
item = function ;
```

### Function
The following rules define how a function is declared.

```ebnf
function = "fn" identifier "(" [parameter_list] ")" ["->", type] block ;
parameter_list = parameter { "," parameter } ;
parameter = identifier ":" type ;
```

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

Aera splits expressions into two types: ones containing block expressions and ones without it. The former includes expressions usually represented as statements in other languages, such as the if, while and for loop expressions.

The latter includes expressions that are expected in programming languages (e.g., a + b).

```ebnf
expression = expression_with_block
            | expression_without_block ;

expression_with_block = block_expression
            | loop_expression
            | if_expression
            | match_expression ;
    
expression_without_block = literal_expression
            | operator_expression
            | grouped_expression
            | call_expression
            | break_expression
            | continue_expression
            | range_expression
            | return_expression ;
```

### Expression With Block

Listed below are the grammar rules for expressions containing block expressions. 

```ebnf
block_expression = "{" { statement } expression "}" ;
loop_expression = infinite_loop_expression;
            | while_loop_expression
            | for_loop_expression ;

infinite_loop_expression = "loop" block_expression ;
while_loop_expression = "while" expression block_expression ; 
for_loop_expression = "for" pattern "in" expression block_expression ;

pattern = range_pattern
            | literal_pattern
            | identifier_pattern ;

range_pattern = range_exclusive_pattern
            | range_inclusive_pattern
            | range_from_pattern
            | range_to_pattern
            | range_to_inclusive_pattern ;

range_exclusive_pattern = literal_pattern ".." literal_pattern ;
range_inclusive_pattern = literal_pattern "..=" literal_pattern ;
range_from_pattern = literal_pattern ".." ;
range_to_pattern = ".." literal_pattern ;
range_to_inclusive_pattern = "..=" literal_pattern ;

literal_pattern = [ "-" ] literal ;

identifier_pattern = identifier ;
```

### Expression Without Block

Listed below are the grammar rules for expressions seen in other languages.

```ebnf
literal_expression = literal ;
operator_expression = assignment_expression
            | compound_assignment_expression
            | comparsion_expression
            | arithmetic_expression
            | bitwise_expression 
            | logical_expression
            | negation_expression 
            | try_propagation_expression
            | fallback_expression
            | type_cast_expression ;

assignment_expression = expression "=" expression ;
compound_assignment_expression = expression "+=" expression
            | expression "-=" expression
            | expression "*=" expression
            | expression "/=" expression
            | expression "%=" expression
            | expression "&=" expression
            | expression "|=" expression
            | expression "^=" expression
            | expression "<<=" expression
            | expression ">>=" expression ;

comparsion_expression = expression "==" expression
            | expression "!=" expression
            | expression ">" expression
            | expression "<" expression
            | expression ">=" expression
            | expression "<=" expression ;

arithmetic_expression = expression "+" expression
            | expression "-" expression
            | expression "*" expression
            | expression "/" expression
            | expression "%" expression ;

bitwise_expression = expression "&" expression
            | expression "|" expression
            | expression "^" expression
            | expression ">>" expression
            | expression "<<" expression ;

logical_expression = expression "&&" expression
            | expression "||" expression ;

negation_expression = "-" expression
            | "!" expression ;

try_propagation_expression = expression "?" ;
fallback_expression = expression "??" ;
type_cast_expression = expression "as" expression ; 

grouped_expression = "(" expression ")" ;
call_expression = "(" [ call_params ] ")" ;
call_params = expression { ","  expression } ;
break_expression = "break" ;
continue_expression = "continue" ;
range_expression = range_from_to_expression
            | range_from_expression
            | range_to_expression
            | range_full_expression
            | range_inclusive_expression
            | range_to_inclusive_expression ;

range_from_to_expression = expression ".." expression;
range_from_expression = expression ".." ;
range_to_expression = ".." expression ; 
range_full_expression = ".." ;
range_inclusive_expression = expression "..=" expression ;
range_to_inclusive_expression = "..=" expression ;

return_expression = "return" [ expression ] ;
```

## Statements

Statements introduce or control structure in Aera, as opposed to statements in other familiar languages.
Let statements are immutable, and mutability will be added once the language further develops.

```ebnf
statement = let_statement
            | const_statement
            | expression_statement ;


let_statement = "let" identifier [ ":" type ] [ "=" expression ] ;
const_statement = "const" identifier [ ":" type ] "=" expression ;
expression_statement = expression ;
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
