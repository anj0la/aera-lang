open Frontend.Token
open Value

module StringMap = Map.Make(String)

type environment = {
	bindings: value StringMap.t
}

let bind name value env = (* binds a new name to a value -> IMMUTABLE, so no reassignment *)
	StringMap.add name value env
