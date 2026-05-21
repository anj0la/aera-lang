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
    (peek par).kind <> EOF

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
    | IntLiteral value      -> Ok (Literal (LitInt value), par')
    | FloatLiteral value    -> Ok (Literal (LitFloat value), par')
    | CharLiteral c         -> Ok (Literal (LitChar c), par')
    | StringLiteral str     -> Ok (Literal (LitString str), par')
    | True                  -> Ok (Literal (LitBool true), par')
    | False                 -> Ok (Literal (LitBool false), par')
    (* Identifier *)
    | Identifier str        -> Ok (Identifier str, par')
    (* Prefix Operators *)
    | Minus                 -> let rhs = par' |> expr_bp (prefix_bp Neg) in
                                    (match rhs with
                                    | Error e -> Error e
                                    | Ok (rhs', par'') -> Ok (Unary ({op = Neg; rhs = rhs'}), par''))
                                   
    | Exclaim               -> let rhs = par' |> expr_bp (prefix_bp Not) in
                                    (match rhs with
                                    | Error e -> Error e
                                    | Ok (rhs', par'') -> Ok (Unary ({op = Not; rhs = rhs'}), par''))
    (* Invalid Token *)             
    | _                     -> Error("unsupported token in language.", tok, par') in
    (* Infix Operators *)
    match lhs with 
    | Error e -> Error e
    | Ok (lhs', par'') -> loop lhs' min_bp par''

and loop lhs min_bp par =
    let tok = peek par in 
    match tok.kind with 
    | EOF   -> Ok (lhs, par)
    | kind  -> begin 
                match par |> get_binary_op kind with
                | None       -> Error("unsupported token in language.", tok, par)
                | Some op    -> begin
                                    match infix_bp op with
                                    | None      -> Ok (lhs, par)
                                    | Some (left_bp, right_bp) ->
                                        if left_bp < min_bp then Ok (lhs, par)
                                        else
                                            let (_, par') = next par in
                                            let res = par' |> expr_bp right_bp in
                                            begin 
                                                match res with
                                                | Error e -> Error e
                                                | Ok (rhs, par'') -> let lhs' = Binary ({lhs = lhs; op = op; rhs = rhs}) in
                                                                match par'' |> loop lhs' min_bp with
                                                                | Error e -> Error e
                                                                | Ok (lhs'', par''') -> Ok (lhs'', par''')
                                            end
                                end
                                            

                end

let expr par =
    par |> expr_bp 0 


(* Statements *)

let expect_identifier par =
    let (tok, par') = next par in
    match tok.kind with 
    | Identifier str        -> Ok(str, par')
    | _                     -> Error ("expected identifier name", tok, par)


let parse_type par =
    let (_, par') = next par in (* consume : token *)
    match expect_identifier par' with (* consume type -> identifier in parser*)
    | Error e -> Error e
    | Ok (typ, par'') -> Ok (typ, par'')


let parse_expr par = 
    let (_, par') = next par in (* consume = token *)
    match expr par' with 
    | Error e -> Error e
    | Ok (expr', par') -> Ok (expr', par')

let let_stmt par =
    match expect_identifier par with (* advance to the next token *)
    | Error e -> Error e
    | Ok (name, par') -> 
        begin
            match (peek par').kind with 
            | Colon ->  begin 
                            match parse_type par' with
                            | Error e -> Error e
                            | Ok (typ, par'') -> begin 
                                                    match (peek par'').kind with 
                                                    | Equal -> begin
                                                                    match parse_expr par'' with 
                                                                    | Error e -> Error e
                                                                    | Ok (expr', par''') -> 
                                                                        Ok (LetStmt { name = name; typ = Some typ; expr = Some expr' }, par''')
                                                                end
                                                    | _ -> Ok (LetStmt { name = name; typ = Some typ; expr = None }, par'')
                                                end
                        end
            | Equal -> begin
                            match parse_expr par' with 
                            | Error e -> Error e
                            | Ok (expr', par'') -> Ok (LetStmt { name = name; typ = None; expr = Some expr' }, par'')
                        end
            | _ -> Ok (LetStmt { name = name; typ = None; expr = None }, par')

        end
                        
let const_stmt par = match expect_identifier par with (* advance to the next token *)
    | Error e -> Error e
    | Ok (name, par') -> 
        let tok = (peek par') in 
        begin 
            match tok.kind with 
            | Colon -> begin 
                            match parse_type par' with
                            | Error e -> Error e
                            | Ok (typ, par'') -> let tok' = (peek par'') in
                                                    begin 
                                                        match tok'.kind with 
                                                        | Equal -> begin
                                                                        match parse_expr par'' with 
                                                                        | Error e -> Error e
                                                                        | Ok (expr', par''') -> 
                                                                            Ok (ConstStmt { name = name; typ = Some typ; expr = expr' }, par''')
                                                                    end
                                                        | _ -> Error ("expected expression after '='", tok', par'')
                                                    end
                        end
            | Equal -> begin
                            match parse_expr par' with 
                            | Error e -> Error e
                            | Ok (expr', par'') -> Ok (ConstStmt { name = name; typ = None; expr = expr' }, par'')
                        end
            | _ -> Error ("expected '=' for constant declaration", tok, par')
        end


let expr_stmt par =
    match expr par with 
    | Error e -> Error e
    | Ok (expr, par) -> Ok (ExprStmt expr, par)

let stmt par = 
    match (peek par).kind with
    | Let -> (match let_stmt par with
            | Error e -> Error e
            | Ok (let_stmt, par') -> Ok(let_stmt, par'))
    | Const -> (match const_stmt par with
            | Error e -> Error e
            | Ok (const_stmt, par') -> Ok(const_stmt, par'))       
    | _  -> (match expr_stmt par with 
            | Error e -> Error e
            | Ok (expr_stmt, par') -> Ok(expr_stmt, par'))

     
(* Parse Function *)

let rec parse_helper stmts par = 
    if is_at_end par then 
        (stmts, par)
    else
        match stmt par with 
        | Error (msg, tok, par') -> (stmts, { par' with reporter = add_error tok.pos msg par'.reporter }) (* need better error handling *)
        | Ok (stmt, par') -> par' |> parse_helper (stmt :: stmts) 

let parse par =
    par |> parse_helper []