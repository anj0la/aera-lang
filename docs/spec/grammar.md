# Grammar

Aera's grammar is used to parse the sequence of tokens into the abstract syntax tree. It starts off with the first rule which matches an entire Aera program.

```ebnf
program = { declaration } EOF ;
```

## Declarations
A program is a sequence of declarations, which are the statements that bind new identifiers or any of the statement types.

```ebnf
declaration = function_declaration
            | variable_declaration
            | const_declaration
            | struct_declaration 
            | class_declaration
            | trait_declaration 
            | with_declaration ;
```

### Function Declaration
The following rules define how a function is declared.

```ebnf
function_declaration = { decorator } [visibility_modifier] [behaviour_modifier] "fn" identifier "(" [parameter_list] ")" ["->", type] block ;
decorator = "@" identifier ;
visibility_modifier = "pub" ;
behaviour_modifier = "modifies" ;
parameter_list = parameter { "," parameter } ;
parameter = identifier ":" type ;
```

### Variable Declaration
The following rules define how variables and constants are declared.

```ebnf
variable_declaration = "let" ["mut"] identifier [":" type] ["=" expression] ;
const_declaration = "const" identifier [":" type] "=" expression ;
```

### Type Declaration
The following rules define how structs, classes and traits are declared.

```ebnf
struct_declaration = "struct" identifier "{" { field_declaration } "}" ;
field_declaration = identifier ":" type ["=" expression] ;

class_declaration = "class" identifier [":" identifier] class_body ;
class_body = "{" { class_member } "}";
class_member = field_declaration | function_declaration ;
trait_declaration = "trait" identifier "{" { function_declaration } "}" ;
with_declaration = "with" identifier "for" identifier "{" { function_declaration } "}" ;
```

## Expressions
Expressions produce value in Aera. There are a number of unary and binary operators with different levels of precedence.

### Precedence and Associativity
The following table showcases the operator precedence, along with its associativity rules.

| Precedence | Operators | Description | Associativity |
|------------|-----------|-------------|---------------|
| 1 (Highest) | [], ., (),  ?, ?? | Array indexing, Object accessor, Function call, Optional, Error | Left-to-right |
| 2 | !, ~, -, & |  Unary NOT, Bitwise NOT, Unary Minus, Address-of | Right-to-left |
| 3 | *, /, % | Multiplication, Division, Modulus | Left-to-right |
| 4 | +, - | Addition, Subtraction | Left-to-right |
| 5 | <<, >> | Bitwise shifts | Left-to-right |
| 6 | <, <=, >, >= | Relational operators | Left-to-right |
| 7 | & | Bitwise AND | Left-to-right |
| 8 | ^ | Bitwise XOR | Left-to-right |
| 9 | \| | Bitwise OR | Left-to-right |
| 10 | && | Logical AND | Left-to-right |
| 11 | \|\| | Logical OR | Left-to-right |
| 12 | if … else … | Ternary operator | Left-to-right |
| 13 (Lowest) | =, +=, -=, *=, /=, %=, <<=, >>=, &=, \|=, ^-, ~= | Assignment and compound assignment | Right-to-left |

```ebnf
expression = assignment ;

assignment = expression assignment_op assignment | conditional ;
assignment_op = "=" | "+=" | "-=" | "*=" | "/=" | "%=" | "<<=" | ">>=" | "&=" | "|=" | "^=" | "~=" ;

conditional = logical_or [ "if" expression "else" else conditional ] ;
logical_or = logical_and { "||" logical_and } ;
logical_and = bitwise_or { "&&" bitwise_or } ;
bitwise_or = bitwise_xor { "|" bitwise_xor } ;
bitwise_xor = bitwise_and { "^" bitwise_and } ;
bitwise_and = equality { "&" equality } ;
equality = comparison { ("==" | "!="), comparison } ;
comparison = shift { (">" | ">=" | "<" | "<=") shift } ;
shift = term { (">>" | "<<") term } ;
term = factor { ("+" | "-") factor } ;
factor = unary { ("*" | "/" | "%") unary } ;

unary = ("!" | "-"| "~" | "&" ) unary | cast ;
cast = postfix ["as" type] ; 
postfix = primary { ("[" expression "]" | "." identifier | "(" [argument_list] ")"  | "?" | "??" ) } ;
primary = literal | identifier | "(" expression ")" ;

argument_list = expression { ","  expression } ;
```

## Statements

```ebnf
statement = variable_declaration
            | const_declaration
            | expression_statement
            | return_statement
            | if_statement
            | while_statement
            | for_statement
            | loop_statement
            | block ;

expression_statement = expression ;
return_statement = "return" [expression] ;
if_statement = "if" expression statement ["else" statement] ;
while_statement = "while" expression statement ;
for_statement = "for" identifier "in" expr [".." expr] statement ; 
loop_statement = "loop" statement ;
block = "{" { statement } "}" ;
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
hexadecimal_integer = "0" ("x" | "X") hex_digit { hex_digit } ;
binary_integer = "0" ("b" | "B") binary_digit { binary_digit } ;
octal_integer = "0" ("o" | "O") octal_digit { octal_digit } ;
float_literal = decimal_integer "." decimal_integer [ scientific_notation ]
              | decimal_integer scientific_notation ;
scientific_notation = ("e" | "E") ["+" | "-"] decimal_integer ;

float_literal = integer_literal "." integer_literal ;
character_literal = "'" character  "'" ";" ;
string_literal = """ { character } """ ";" ;
boolean_literal = "true" | "false" ;

character = printable | escape_sequence ;
printable = alpha | digit | symbol | space ;
alpha = "a" ... "z" | "A" ... "Z" ;
digit = "0" ... "9";
hex_digit = digit | 
digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" | "8" | "9" ;
hex_digit = digit | "a" ... "f" | "A" ... "F" ;
binary_digit = "0" | "1" ;
octal_digit = "0" | "1" | "2" | "3" | "4" | "5" | "6" | "7" ;
symbol = "!" ... "/" | ":" ... "@" | "[" ... "`" | "{" ... "~" ;
underscore = "_" ;
escape_sequence = "\\" ( "n" | "t" | "r" | "\"" | "'" | "\\" ) ;
space = " " ;

identifier = (alpha  | "_" ) { (alpha | digit | "_" ) } ;

type = primitive_type 
     | user_type 
     | dynamic_array_type
     | map_type
     | generic_type 
     | static_array_type ; 

primitive_type =  "int8" | "int16" | "int32" | "int64" | "uint8" | "uint16" | "uint32" | "uint64" | "float32" | "float64" | "char" | "string" | "bool" ; 
user_type = identifier ; 
dynamic_array_type = identifier !" "<" type ">" ; 
map_type = identifier "!" "<" type "," type ">" ;
generic_type = identifier !" "<" type_list ">" ; 
static_array_type = type array_dimensions ;

array_dimensions = ( "[" [integer_literal] "]" ) { ( "[" [integer_literal] "]" ) } ;
type_list = type { "," type } ;
```