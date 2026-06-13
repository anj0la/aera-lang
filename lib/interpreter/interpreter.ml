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
    | Binary { lhs; op; rhs }                           -> (match eval_binary lhs op rhs with 
                                                            | Error e -> Error e
                                                            | Ok value -> Ok value)
    (* | Unary { op; rhs }                                 -> *)
    | _ -> failwith "not implemented yet"


and eval_binary lhs op rhs =
    let l = eval lhs in 
    let r = eval rhs in 
    match l, r with
    | Int64.t a, Int64.t b -> eval_binary_int a op b
    | Float a, Float b -> eval_binary_float a op b
    | Char a, Char b -> eval_binary_char a op b
    | String a, String b -> eval_binary_string a op b
    | Bool a, Bool b -> eval_binary_bool a op b
    | _ -> Error ("error: expected int, float, char, string or bool literal")

and eval_binary_int lhs op rhs =
    match op with
    | Add -> Ok (VInt (lhs + rhs))
    | Sub -> Ok (VInt (lhs - rhs))
    | Mul -> Ok (VInt (lhs * rhs))
    | Div -> if rhs = 0 then Error ("error: division by zero is undefined")
            else
                Ok (VInt (lhs / rhs))
    | Mod -> if rhs = 0 then Error ("error: division by zero is undefined")
            else
                Ok (VInt (lhs % rhs))     
    | Eq -> Ok (VBool (lhs = rhs))
    | Neg -> Ok (VBool (lhs <> rhs))
    | Lt -> Ok (VBool (lhs < rhs))
    | Lte -> Ok (VBool (lhs <= rhs))
    | Gt -> Ok (VBool (lhs > rhs))
    | Gte -> Ok (VBool (lhs >= rhs))
    | And -> Ok (VBool (lhs && rhs))
    | Or -> Ok (VBool (lhs || rhs))
    | BitAnd -> Ok (VInt (lhs land rhs))
    | BitOr -> Ok (VInt (lhs lor rhs))
    | BitXor -> Ok (VInt (lhs lxor rhs))
    | Shl -> Ok (VInt (lhs lsl rhs))
    | Shr -> Ok (VInt (lhs asr rhs))

and eval_binary_float lhs op rhs =
    match op with
    | Add -> Ok (VFloat (lhs +. rhs))
    | Sub -> Ok (VFloat (lhs -. rhs))
    | Mul -> Ok (VFloat (lhs *. rhs))
    | Div -> if rhs = 0.0 then Error ("error: division by zero is undefined")
            else
                Ok (VFloat (lhs /. rhs))
    | Mod -> if rhs = 0 then Error ("error: division by zero is undefined")
            else
                Ok (VFloat (lhs %. rhs))     
    | Eq -> Ok (VBool (lhs = rhs))
    | Neg -> Ok (VBool (lhs <> rhs))
    | Lt -> Ok (VBool (lhs < rhs))
    | Lte -> Ok (VBool (lhs <= rhs))
    | Gt -> Ok (VBool (lhs > rhs))
    | Gte -> Ok (VBool (lhs >= rhs))
    | And -> Ok (VBool (lhs && rhs))
    | Or -> Ok (VBool (lhs || rhs))
    | _ -> Error ("error: invalid operation on a floating point value")

and eval_binary_char lhs op rhs =
    match op with    
    | Eq -> Ok (VBool (lhs = rhs))
    | Neg -> Ok (VBool (lhs <> rhs))
    | _ -> Error ("error: invalid operation on a char value")

and eval_binary_string lhs op rhs =
    match op with
    | Add -> Ok (VString (lhs ^ rhs))
    | Eq -> Ok (VBool (lhs = rhs))
    | Neg -> Ok (VBool (lhs <> rhs))
    | _ -> Error ("error: invalid operation on a string value")

and eval_binary_bool lhs op rhs =
    match op with
    | Eq -> Ok (VBool (lhs = rhs))
    | Neg -> Ok (VBool (lhs <> rhs))
    | Lt -> Ok (VBool (lhs < rhs))
    | Lte -> Ok (VBool (lhs <= rhs))
    | Gt -> Ok (VBool (lhs > rhs))
    | Gte -> Ok (VBool (lhs >= rhs))
    | And -> Ok (VBool (lhs && rhs))
    | Or -> Ok (VBool (lhs || rhs))
    | _ -> Error ("error: invalid operation on a boolean value")