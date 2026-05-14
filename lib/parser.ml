open Ast
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
    | h :: _ -> h
    | [] -> failwith "the parser cannot be empty."

let peek_next par = 
    match par.tokens with
    | _ :: s :: _ -> s
    | _ -> failwith "the parser cannot be empty."

let is_at_end par =
    let token = peek par in token.kind <> EOF

let advance par =
    match par.tokens with
    | _ :: t -> { par with tokens = t }
    | [] -> par

let next par = 
    (peek par, advance par)

let check kind par = 
    if is_at_end par then (* checks that the token is NOT EOF *)
        false
    else
        let token = peek par in token.kind = kind

let check_next kind par =  (* checks that there is NO token after EOF *)
    if is_at_end par then
        false
    else
        let token = peek_next par in token.kind = kind

let consume kind par = 
    if check kind par then 
        Some (advance par)
    else
        None

(* Expressions *)
    
let prefix_bp op =
    match op with 
    | Neg | Not -> 20

let infix_bp op =
    match op with
    | Eq -> Some (2, 1) (* format = (left binding power, right binding power) *)
    | Or -> Some (3, 4)
    | And -> Some (5, 6)
    | Lt | Lte | Gt | Gte -> Some (7, 8)
    | Add | Sub -> Some (9, 10)
    | Mul | Div | Mod -> Some (11, 12)
    | _ -> None

let get_binary_op tok_kind par =
    match tok_kind with
    | Plus          -> Some Add
    | Minus         -> Some Sub
    | Star          -> Some Mul
    | Slash         -> Some Div
    | Percent       -> Some Mod
    | Less          -> Some Lt
    | LessEqual     -> Some Lte
    | Greater       -> Some Gt
    | GreaterEqual  -> Some Gte
    | _             -> None

let rec expr_bp min_bp par =
    let (tok, par') = next par in 
    let lhs = match tok.kind with
    (* Literals *)
    | IntLiteral value      -> Ok (Literal (LitInt value))
    | FloatLiteral value    -> Ok (Literal (LitFloat value))
    | CharLiteral c         -> Ok (Literal (LitChar c))
    | StringLiteral str     -> Ok (Literal (LitString str))
    | True                  -> Ok (Literal (LitBool true))
    | False                 -> Ok (Literal (LitBool false))
    (* Identifier *)
    | Identifier str        -> Ok (Identifier str)
    (* Prefix Operators *)
    | Minus                 -> let rhs = par' |> expr_bp (prefix_bp Neg) in
                                    (match rhs with
                                    | Error e -> Error e
                                    | Ok rhs' -> Ok (Unary ({op = Neg; rhs = rhs'})))
                                   
    | Exclaim               -> let rhs = par' |> expr_bp (prefix_bp Not) in
                                    (match rhs with
                                    | Error e -> Error e
                                    | Ok rhs' -> Ok (Unary ({op = Not; rhs = rhs'})))
    (* Invalid Token *)             
    | _                     -> Error("unsupported token in language.") in
    (* Infix Operators *)
    match lhs with 
    | Error e -> Error e
    | Ok lhs' -> loop lhs' min_bp par'

and loop lhs min_bp par =
    match (peek par).kind with 
    | EOF   -> Ok (lhs)
    | kind  -> begin 
                match par |> get_binary_op kind with
                | None       -> Error("unsupported token in language.")
                | Some op    -> begin
                                    match infix_bp op with
                                    | None      -> Ok (lhs)
                                    | Some (left_bp, right_bp) ->
                                        if left_bp < min_bp then Ok (lhs)
                                        else
                                            let (_, par') = next par in
                                            let rhs = par' |> expr_bp right_bp in
                                            begin 
                                                match rhs with
                                                | Error e   -> Error e
                                                | Ok rhs'   -> let lhs' = Binary ({lhs = lhs; op = op; rhs = rhs'}) in
                                                                match par' |> loop lhs' min_bp with
                                                                | Error e -> Error e
                                                                | Ok lhs'' -> Ok lhs''
                                            end
                                end
                                            

                end

let expr par =
    match par |> expr_bp 0 with 
    | Error e -> Error e
    | Ok expr -> Ok expr



  