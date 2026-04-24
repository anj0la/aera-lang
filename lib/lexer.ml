open Source_location
open Token

type lexer = {
    source: string;
    start: int;
    curr: int;
    loc: source_location;
}

let is_at_end lex =
    lex.curr >= String.length lex.source

let peek lex = 
    if is_at_end lex then
        None
    else
        Some lex.source.[lex.curr] (* returns the token at the current pointer *)

let advance lex =
    let c = lex.source.[lex.curr] in 
    (c, { lex with curr = lex.curr + 1 }) (* creates a NEW lexer record with the updated current pointer*)

let make_token lex kind =
    let text = 
        String.sub lex.source lex.start (lex.curr - lex.start) in
    { kind = kind; lexeme = text; loc = lex.loc }

let rec read_line_comment lex = 
    if peek lex == Some '\n' || is_at_end lex then ()
    else
        let (_, lex') = advance lex in
        read_line_comment lex'

let is_digit c = 
    c >= '0' && c <= '9'

let is_hex_digit c =
    is_digit c || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')

let is_binary_digit c =
    c == '0' || c == '1'

let is_octal_digit c =
	c >= '0' && c <= '7'

let is_alpha c =
    (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')

let is_alnum c =
    is_alpha c || is_digit c || c == '_'

let is_symbol c =
    (c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~')

let is_space c =
    c == ' '
	
let is_printable c =
    is_alpha c || is_digit c || is_symbol c || is_space c