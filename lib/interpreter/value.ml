module StringMap = Map.Make(String)

type value = 
| VInt of int
| VFloat of float
| VChar of char
| VString of string
| VBool of bool
| VUnit

and environment = value StringMap.t list
