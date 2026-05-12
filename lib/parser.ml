open Error
open Lexer
open Token
   
type parser = {
    tokens: token list;
    reporter: reporter;
    curr: int;
}

(* Helper Functions *)

let peek par =
    match par.tokens with 
    | h :: _ -> Some h
    | [] -> None

let peek_next par = 
    match par.tokens with
    | _ :: s :: _ -> Some s
    | _ -> None

let is_at_end par =
    match peek par with
    | Some token -> token.kind <> EOF
    | None -> true

let advance par =
    match par.tokens with
    | _ :: t -> { par with tokens = t }
    | [] -> par


let check kind par = 
    if is_at_end par then (* checks that the token is NOT EOF*)
        false
    else
        match peek par with
        | Some token -> token.kind = kind
        | None -> false

let check_next kind par =  (* checks that there is NO token after EOF *)
    if is_at_end par then
        false
    else
        match peek_next par with
        | Some token -> token.kind = kind
        | None -> false

let consume kind par = 
    if check kind par then 
        Some (advance par)
    else
        None

(* Expressions *)