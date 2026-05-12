open Token

(* Basic Operators *)

type binary_op =
| Add | Sub | Mul | Div | Mod
| Eq | Neq | Lt | Lte | Gt | Gte
| And | Or

type assign_op = (* trivial right now to get the compiler to work *)
| Assign               

type unary_op =
| Neg | Not

(* Literals *)

type literal_suffix =
| Int8 | Int16 | Int32 | Int64
| UInt8 | UInt16 | UInt32 | UInt64
| Float32 | Float64
| NoSuffix (* bare int / float, inferred later *)

type literal =
| LitInt of Int64.t * literal_suffix (* resolve type later -> TODO: Add support for binary, hexadecimal and octadecimal *)
| LitFloat of float * literal_suffix (* resolve type later *)
| LitChar of char
| LitString of string
| LitBool of bool
| Unit (* represented as () *)

(* Types -> simple, only includes int64 / float64, will resolve later *)

type typ =
| TypNamed of string (* includes int8 -> int64, string, and identifiers *)

(* Program *)

type program = {
    items: item list;
}

and item =
| FnItem of fn_item

and fn_item = {
    name: string;
    params: (string * typ) list;
    return_type: typ option; (* if omitted, return unit type *)
    body: block;
}

(* Expressions *)

and block = {
    stmts: stmt list;
    expr: expr option; 
}

and expr =
| Literal      of literal
| Identifier   of string
| Grouping     of expr
| FnCall       of { callee: expr; args: expr list }
| Binary       of { lhs: expr; op: binary_op; rhs: expr }
| Unary        of { op: unary_op; rhs: expr }
| Assign       of { lhs: expr; op: assign_op; rhs: expr }
| Block        of block
| InfiniteLoop of block
| WhileLoop    of { cond: expr; body: block }
| IfExpr       of { cond: expr; then_branch: block; else_branch: block option }
| Break
| Continue
| Return       of expr option

(* Statements *)

and stmt = 
| Item         of item
| LetStmt      of let_stmt
| ConstStmt    of const_stmt
| ExprStmt     of expr

and let_stmt = { 
    name: string;
    typ: typ option;
    expr: expr option;
}

and const_stmt = {
    name: string;
    typ: typ option;
    expr: expr;
}