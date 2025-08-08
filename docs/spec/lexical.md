# Lexical Structure

## Character Set
Aera source files are encoded in UTF-8.

## Identifiers
An identifier starts with an alphabetic character or an underscore (_), followed by any number of alphanumeric characters or underscores. They are case-sensitive.
Regex: [a-zA-Z_][a-zA-Z0-9_]

## Keywords

The following are reserved keywords, and cannot be used as identifiers:

- fn
- let
- mut
- const
- pub
- modifies
- if
- else
- 
Control flow: if, else, for, while, loop, match, break, continue, return
Reserved: import, class, struct, enum, trait, alias, self, as, bind

Note that as the language expands, so will the reserved keywords.

## Symbolic Tokens

### Operator Tokens
Listed below are the following valid operators in Aera:

| Token | Description                        |
| ----- | ---------------------------------- |
| +     | Addition                           |
| -     | Subtraction                        |
| \*    | Multiplication                     |
| /     | Division                           |
| %     | Modulus                            |
| &&    | Logical AND                        |
| \|\|  | Logical OR                         |
| !     | Logical NOT                        |
| &     | Address-of and Bitwise AND         |
| \|    | Bitwise OR                         |
| ^     | Bitwise XOR                        |
| \~    | Bitwise NOT                        |
| >>    | Arithmetic and Logical Left-shift  |
| <<    | Arithmetic and Logical Right-shift |
| =     | Assignment and Initialization      |
| ++    | Increment                          |
| --    | Decrement                          |
| +=    | Add-and-assign                     |
| -=    | Subtract-and-assign                |
| \*=   | Multiply-and-assign                |
| /=    | Divide-and-assign                  |
| %=    | Modulus-and-assign                 |
| &=    | Bitwise AND-and-assign             |
| \|=   | Bitwise OR-and-assign              |
| ^=    | Bitwise XOR-and-assign             |
| \~=   | Bitwise NOT-and-assign             |
| <<=   | Left-shift-and-assign              |
| >>=   | Right-shift-and-assign             |
| ==    | Equality / Equal to                |
| !=    | Inequality / Not equal to          |
| >     | Greater than                       |
| >=    | Greater than or equal to           |
| <     | Less than                          |
| <=    | Less than or equal to              |
| ?     | Optional type                      |
| ??    | Error type                         |
| ..    | Exclusive range                    |
| ..=   | Inclusive range                    |

### Punctuation & Separator Tokens
Listed below are the following punctuation and separator tokens used in Aera:

| Token    | Description                                                                               |
| -------- | ----------------------------------------------------------------------------------------- |
| \[ and ] | Subscript and deduced parameter lists                                                     |
| ( and )  | Function call / declarations and tuple literals                                           |
| { and }  | Blocks of control flow statements, struct literals, body definitions (classes, functions) |
| ,        | Separate tuple and array elements                                                         |
| .        | Member access                                                                             |
| :        | Name binding patterns                                                                     |
| ; or \n  | Statement separator                                                                       |

### Other Tokens
Listed below are some other tokens used in Aera that don’t fit into the above categories:

| Token | Description        |
| ----- | ------------------ |
| ->    | Return type        |
| =>    | Match syntax       |
| @     | Compiler directive |

## Comments
Single-line comments begin with **#** and continue to the end of the line. Multi-line comments begin with an opening **<#** and a closing **#>**.

## Whitespace
Spaces and tabs are used as token separators and are otherwise ignored.