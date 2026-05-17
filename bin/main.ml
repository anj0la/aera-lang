open Aera

let token_to_string kind =
  match kind with
  | Token.IntLiteral num 		-> Printf.sprintf "int(%Ld)" num
  | Token.FloatLiteral num		-> Printf.sprintf "float(%f)" num
  | Token.StringLiteral str		-> Printf.sprintf "str(%s)" str
  | Token.CharLiteral ch		-> Printf.sprintf "char(%c)" ch
  | Token.Identifier str 		-> Printf.sprintf "identifier(%s)" str
  | Token.Let					-> "let"
  | Token.Fn					-> "fn"
  | Token.Equal					-> "="
  | _							-> ""
  (* etc *)
	
let () = let lex: Lexer.lexer = {
	source = "let x = \"hello world\"";
	start = 0;
	curr = 0;
	pos = { line = 1; col = 1 };
	tokens = [];
	reporter = { errors = [] };
	}
	in let lex' = Lexer.read_tokens lex in
List.iter (fun tok -> print_endline (token_to_string tok.Token.kind)) lex'.tokens
