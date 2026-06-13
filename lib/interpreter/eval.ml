open Frontend
open Value

let rec eval expr =
    match expr with 
    | Frontend.Ast.Literal (LitInt n)                   -> Ok (VInt n)
    | Literal (LitFloat f)                              -> Ok (VFloat f)
    | Literal (LitChar c)                               -> Ok (VChar c)
    | Literal (LitString s)                             -> Ok (VString s)
    | Literal (LitBool b)                               -> Ok (VBool b)
    | Grouping expr'                                    -> eval expr'
    | Binary { lhs; op; rhs }                           -> eval_binary lhs op rhs         
    | Unary { op; rhs }                                 -> eval_unary op rhs
    | _ -> failwith "not implemented yet"

and eval_binary lhs op rhs =
    let l = eval lhs in 
    let r = eval rhs in 
    match l, r with
    | Ok (VInt a), Ok (VInt b)                          -> eval_binary_int a op b
    | Ok (VFloat a), Ok (VFloat b)                      -> eval_binary_float a op b
    | Ok (VChar a), Ok (VChar b)                        -> eval_binary_char a op b
    | Ok (VString a), Ok (VString b)                    -> eval_binary_string a op b
    | Ok (VBool a), Ok (VBool b)                        -> eval_binary_bool a op b
    | _                                                 -> Error ("error: expected int, float, char, string or bool literal")

and eval_binary_int lhs op rhs =
    match op with
    | Add       -> Ok (VInt (lhs + rhs))
    | Sub       -> Ok (VInt (lhs - rhs))
    | Mul       -> Ok (VInt (lhs * rhs))
    | Div       -> if rhs = 0 then Error ("error: division by zero is undefined")
                    else
                    Ok (VInt (lhs / rhs))
    | Mod       -> if rhs = 0 then Error ("error: division by zero is undefined")
                    else
                    Ok (VInt (lhs mod rhs))     
    | Eq        -> Ok (VBool (lhs = rhs))
    | Neq       -> Ok (VBool (lhs <> rhs))
    | Lt        -> Ok (VBool (lhs < rhs))
    | Lte       -> Ok (VBool (lhs <= rhs))
    | Gt        -> Ok (VBool (lhs > rhs))
    | Gte       -> Ok (VBool (lhs >= rhs))
    | BitAnd    -> Ok (VInt (lhs land rhs))
    | BitOr     -> Ok (VInt (lhs lor rhs))
    | BitXor    -> Ok (VInt (lhs lxor rhs))
    | Shl       -> Ok (VInt (lhs lsl rhs))
    | Shr       -> Ok (VInt (lhs asr rhs))
    | _         -> Error ("error: invalid operation on an int value")

and eval_binary_float lhs op rhs =
    match op with
    | Add       -> Ok (VFloat (lhs +. rhs))
    | Sub       -> Ok (VFloat (lhs -. rhs))
    | Mul       -> Ok (VFloat (lhs *. rhs))
    | Div       -> if rhs = 0.0 then Error ("error: division by zero is undefined")
                    else
                    Ok (VFloat (lhs /. rhs))
    | Mod       -> if rhs = 0.0 then Error ("error: division by zero is undefined")
                    else
                    Ok (VFloat (mod_float lhs rhs))     
    | Eq        -> Ok (VBool (lhs = rhs))
    | Neq       -> Ok (VBool (lhs <> rhs))
    | Lt        -> Ok (VBool (lhs < rhs))
    | Lte       -> Ok (VBool (lhs <= rhs))
    | Gt        -> Ok (VBool (lhs > rhs))
    | Gte       -> Ok (VBool (lhs >= rhs))
    | _         -> Error ("error: invalid operation on a floating point value")

and eval_binary_char lhs op rhs =
    match op with    
    | Eq        -> Ok (VBool (lhs = rhs))
    | Neq       -> Ok (VBool (lhs <> rhs))
    | _         -> Error ("error: invalid operation on a char value")

and eval_binary_string lhs op rhs =
    match op with
    | Add       -> Ok (VString (lhs ^ rhs))
    | Eq        -> Ok (VBool (lhs = rhs))
    | Neq       -> Ok (VBool (lhs <> rhs))
    | _         -> Error ("error: invalid operation on a string value")

and eval_binary_bool lhs op rhs =
    match op with
    | Eq        -> Ok (VBool (lhs = rhs))
    | Neq       -> Ok (VBool (lhs <> rhs))
    | Lt        -> Ok (VBool (lhs < rhs))
    | Lte       -> Ok (VBool (lhs <= rhs))
    | Gt        -> Ok (VBool (lhs > rhs))
    | Gte       -> Ok (VBool (lhs >= rhs))
    | And       -> Ok (VBool (lhs && rhs))
    | Or        -> Ok (VBool (lhs || rhs))
    | _         -> Error ("error: invalid operation on a boolean value")

and eval_unary op rhs = 
    let r = eval rhs in 
    match r with
    | Ok (VInt n)       -> n |> eval_unary_int op
    | Ok (VFloat f)     -> f |> eval_unary_float op
    | Ok (VBool b)      -> b |> eval_unary_bool op
    | _ -> Error ("error: invalid unary operation")


and eval_unary_int op rhs =
    match op with 
    | Neg -> Ok (VInt (-rhs))
    | _ -> Error ("error: invalid unary operation on an int value")

and eval_unary_float op rhs =
    match op with 
    | Neg -> Ok (VFloat (-.rhs))
    | _ -> Error ("error: invalid unary operation on a floating point value")

and eval_unary_bool op rhs =
    match op with 
    | Not -> Ok (VBool (not rhs))
    | _ -> Error ("error: invalid unary operation on a floating point value")