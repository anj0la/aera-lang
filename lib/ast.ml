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

and expr =
| Literal      of literal
| Identifier   of string
| Binary       of { lhs: expr; op: binary_op; rhs: expr }
| Unary        of { op: unary_op; rhs: expr }

(* Statements *)

and stmt = 
| LetStmt      of let_stmt
| ConstStmt    of const_stmt
| ExprStmt     of expr

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