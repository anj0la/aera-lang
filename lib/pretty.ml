open Ast
open Parser


let connector is_last = (* the connector for the node -> if the last node, the connector has a curve *)
    if is_last then
        "└── "
    else 
        "├── "

let child_prefix prefix is_last =
    if is_last then
        prefix ^ "    "
    else
        prefix ^ "│   "

let format_binary_op op =
    match op with 
    | Add       -> "+"
    | Sub       -> "-"
    | Mul       -> "*"
    | Div       -> "/"
    | Mod       -> "%"
    | Eq        -> "=="
    | Neq       -> "!="
    | Lt        -> "<"
    | Lte       -> "<="
    | Gt        -> ">"
    | Gte       -> ">="
    | And       -> "&&"
    | Or        -> "||"
    | BitAnd    -> "&"
    | BitOr     -> "|"
    | BitXor    -> "^"
    | Shl       -> "<<"
    | Shr       -> ">>"

let format_unary_op op =
    match op with 
    | Neg   -> "-"
    | Not   -> "!"

let format_assign_op op =
    match op with 
    | EqAssign          -> "="
    | AddAssign         -> "+="
    | SubAssign         -> "-="
    | MulAssign         -> "*="
    | DivAssign         -> "/="
    | ModAssign         -> "%="
    | AndAssign         -> "&="
    | PipeAssign        -> "|="
    | CaretAssign       -> "^="
    | ShlAssign         -> "<<="
    | ShrAssign         -> ">>="

let format_literal prefix is_last value =
    [ Printf.sprintf "%s %s Literal (\"%s\")" prefix (connector is_last) value ]

let format_identifier prefix is_last value = 
    [ Printf.sprintf "%s %s Identifier (\"%s\")" prefix (connector is_last) value]

let rec format_expr prefix is_last expr = 
    match expr with
    | Literal (LitInt n)                        -> format_literal prefix is_last (Int64.to_string n)
    | Literal (LitFloat f)                      -> format_literal prefix is_last (string_of_float f)
    | Literal (LitChar c)                       -> format_literal prefix is_last (String.make 1 c)
    | Literal (LitString s)                     -> format_literal prefix is_last s
    | Literal (LitBool b)                       -> format_literal prefix is_last(Bool.to_string b)
    | Identifier s                              -> format_identifier prefix is_last s
    | Grouping expr'                            -> format_grouping expr' prefix is_last
    | Binary { lhs; op; rhs }                   -> format_binary lhs op rhs prefix is_last

and format_grouping expr prefix is_last =
    let group_lst = [ Printf.sprintf "%s %s Group" prefix (connector is_last) ] in 
    let expr_lst = expr |> format_expr (child_prefix prefix is_last) true in
    group_lst @ expr_lst

and format_binary lhs op rhs prefix is_last =
    let binary_lst = [ Printf.sprintf "%s %s Binary op=\"%s\"" prefix (connector is_last) (format_binary_op op) ] in 
    let lhs_lst = lhs |> format_expr (child_prefix prefix is_last) false in
    let rhs_lst = rhs |> format_expr (child_prefix prefix is_last) true in
    binary_lst @ lhs_lst @ rhs_lst

and format_assign lhs op rhs prefix is_last =
    let assign_lst = [ Printf.sprintf "%s %s Assign op=\"%s\"" prefix (connector is_last) (format_assign_op op) ] in 
    let lhs_lst = lhs |> format_expr (child_prefix prefix is_last) false in
    let rhs_lst = rhs |> format_expr (child_prefix prefix is_last) true in
    assign_lst @ lhs_lst @ rhs_lst

and format_unary op rhs prefix is_last = 
    let unary_lst = [ Printf.sprintf "%s %s Unary op=\"%s\"" prefix (connector is_last) (format_unary_op op) ] in
    let rhs_lst = rhs |> format_expr (child_prefix prefix is_last) true in
    unary_lst @ rhs_lst

and format_infinite_loop expr prefix is_last =
    let infinite_loop_lst = [ Printf.sprintf "%s %s Loop" prefix (connector is_last) ] in 
    let expr_lst = expr |> format_expr (child_prefix prefix is_last) true in
    infinite_loop_lst @ expr_lst

and format_while_loop cond body prefix is_last =
    let while_loop_lst = [ Printf.sprintf "%s %s While" prefix (connector is_last) ] in
    let cond_lst = cond |> format_expr (child_prefix prefix is_last) false in 
    let body_lst = body |> format_expr (child_prefix prefix is_last) true in 
    while_loop_lst @ cond_lst @ body_lst

and format_if cond then_branch else_branch prefix is_last =
    let if_lst = [ Printf.sprintf "%s %s If" prefix (connector is_last) ] in
    let cond_lst = cond |> format_expr (child_prefix prefix is_last) false in
    match else_branch with 
    | Some else_expr -> let then_lst = then_branch |> format_expr (child_prefix prefix is_last) false in 
                        let else_lst = else_expr |> format_expr (child_prefix prefix is_last) true in 
                        if_lst @ cond_lst @ then_lst @ else_lst 
    | None -> let then_lst = then_branch |> format_expr (child_prefix prefix is_last) true in 
    if_lst @ cond_lst @ then_lst

and format_break expr_opt prefix is_last  = 
    let break_lst = [ Printf.sprintf "%s %s Break" prefix (connector is_last) ] in 
    match expr_opt with 
    | Some expr -> let expr_lst = expr |> format_expr (child_prefix prefix is_last) true in
                    break_lst @ expr_lst
    | None -> break_lst

and format_return expr_opt prefix is_last = 
    let return_lst = [ Printf.sprintf "%s %s Return" prefix (connector is_last) ] in 
    match expr_opt with 
    | Some expr -> let expr_lst = expr |> format_expr (child_prefix prefix is_last) true in
                    return_lst @ expr_lst
    | None -> return_lst
(* 


| Literal           of literal
| Identifier        of string
| Grouping          of expr
| Call              of { callee: expr; args: expr list }
| Binary            of { lhs: expr; op: binary_op; rhs: expr }
| Assign            of { lhs: expr; op: assign_op; rhs: expr }
| Unary             of { op: unary_op; rhs: expr }
| Block             of block
| InfiniteLoop      of expr
| WhileLoop         of { cond: expr; body: expr }
| IfExpr            of { cond: expr; then_branch: expr; else_branch: expr option }
| BreakExpr         of expr option
| ReturnExpr        of expr option*)
