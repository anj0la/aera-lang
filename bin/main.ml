open Aera
	
let () = 
(* Build lexer *)
let lex: Lexer.lexer = {
	source = "fn main() { 42 }";
    start = 0;
    curr = 0;
    start_pos = { line = 1; col = 1 };
	pos = { line = 1; col = 1 };
    tokens = [];
    reporter = { Error.errors = [] };
} 
(* Parse tokens*)
in let lex' = Lexer.read_tokens lex in 
let par: Parser.parser = {
    tokens = lex'.tokens;
    reporter = lex'.reporter;
    curr = 0;
} in 
(* Print Ast*)
let (program, _) = Parser.parse par in
Pretty.print_ast program

