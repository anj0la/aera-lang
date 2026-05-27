open Token

(* Basic Operators *)

type binary_op =
| Add | Sub | Mul | Div | Mod
| Eq | Neq | Lt | Lte | Gt | Gte
| And | Or

type unary_op =
| Neg | Not (* - / ! *)

(* Literals *)

type literal =
| LitInt of Int64.t (* resolve suffix and type later *)
| LitFloat of float (* resolve suffix and type later *)
| LitChar of char
| LitString of string
| LitBool of bool

(* Program *)

type program = {
    stmts: stmt list;
}

and fn_item = {
    name: string;
    params: (string * string) list;
    return_type: string option; (* if omitted, return unit type *)
    body: expr;
}

(* Expressions *)

and block = {
    stmts: stmt list;
    expr: expr; 
}

and expr =
| Literal      of literal
| Identifier   of string
| Grouping     of expr
| FnCall       of { callee: expr; args: expr list }
| Binary       of { lhs: expr; op: binary_op; rhs: expr }
| Unary        of { op: unary_op; rhs: expr }
| Block        of block
| InfiniteLoop of expr
| WhileLoop    of { cond: expr; body: expr }
| IfExpr       of { cond: expr; then_branch: expr; else_branch: expr option }
| Break
| Continue
| Return       of expr option

(* Statements *)

and stmt = 
| LetStmt      of let_stmt
| ConstStmt    of const_stmt

and let_stmt = { 
    name: string;
    typ: string option;
    expr: expr option;
}

and const_stmt = {
    name: string;
    typ: string option;
    expr: expr;
}