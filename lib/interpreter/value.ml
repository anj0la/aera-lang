open Int64

type value = 
| VInt of Int64.t
| VFloat of float
| VChar of char
| VString of string
| VBool of bool
| VUnit