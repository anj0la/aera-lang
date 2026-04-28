open Position
open Token

type lexer = {
    source: string;
    start: int;
    curr: int;
    pos: position;
}

let is_digit c = 
    c >= '0' && c <= '9'

let is_hex_digit c =
    is_digit c || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F')

let is_binary_digit c =
    c = '0' || c = '1'

let is_octal_digit c =
	c >= '0' && c <= '7'

let is_alpha c =
    (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')

let is_alnum c =
    is_alpha c || is_digit c || c = '_'

let is_symbol c =
    (c >= '!' && c <= '/') || (c >= ':' && c <= '@') || (c >= '[' && c <= '`') || (c >= '{' && c <= '~')

let is_space c =
    c = ' '
	
let is_printable c =
    is_alpha c || is_digit c || is_symbol c || is_space c

let is_at_end lex =
    lex.curr >= String.length lex.source

let peek lex = 
    if is_at_end lex then
        None
    else
        Some lex.source.[lex.curr] (* returns the character at the current pointer *)

let peek_next lex = 
    if lex.curr + 1 >= String.length lex.source then
        None
    else
        Some lex.source.[lex.curr + 1] (* returns the character 1 position ahead of the current pointer *)

let advance lex =
    let c = lex.source.[lex.curr] in (* creates a NEW lexer record with the updated current pointer and other possible values *)
    match c with 
    | '\n' -> (c, { lex with 
                    curr = lex.curr + 1;
                    pos = { line = lex.pos.line + 1; col = 1 }; })
    | '\t' -> (c, { lex with 
                    curr = lex.curr + 1;
                    pos = { lex.pos with col = lex.pos.col + 4 }; })
    | '\r' -> (c, { lex with 
                    curr = lex.curr + 1;
                    pos = { lex.pos with col = 1 }; })
    | _ -> (c, { lex with 
                    curr = lex.curr + 1;
                    pos = { lex.pos with col = lex.pos.col + 1 }; })

let make_token lex kind =
    let text = 
        String.sub lex.source lex.start (lex.curr - lex.start) in
    { kind = kind; lexeme = text; pos = lex.pos }

let rec read_line_comment lex = 
    if peek lex = Some '\n' || is_at_end lex then lex
    else
        let (_, lex') = advance lex in
        read_line_comment lex'

let rec read_block_comment lex =
    if is_at_end lex then Error lex
    else if peek lex = Some '\n' then Error lex
    else if peek lex = Some '#' && peek_next lex = Some '>' then
        let (_, lex') = advance lex in 
        let (_, lex'') = advance lex' in
        Ok lex''
    else 
        let (_, lex') = advance lex in
        read_block_comment lex'

let resolve_char lex c =
    if c = '\\' && not (is_at_end lex) then
            let (c', lex') = advance lex in
            match c' with
            | 'n' -> Ok ('\n', lex')
            | 't' -> Ok ('\t', lex')
            | 'r' -> Ok ('\r', lex')
            | '\\' -> Ok ('\\', lex')
            | '\'' -> Ok ('\'', lex')
            | '"' -> Ok ('"', lex')
            | _ -> Error ("invalid escape sequence", lex')
        else if not (is_printable c) then
            if c = '\\' then Error ("unterminated escape sequence in character literal", lex)
            else
                Error ("invalid character in literal: " ^ String.make 1 c, lex)
        else
            Ok (c, lex) (* character is a valid printable *)

let rec skip_until_closing_quote lex quote =
    if is_at_end lex then lex
    else if peek lex = Some quote then 
        let (_, lex') = advance lex in lex'
    else
        let (_, lex') = advance lex in
        skip_until_closing_quote lex' quote

let close_char lex c = 
    if peek lex != Some '\'' then
        if is_at_end lex then Error ("unterminated character literal", lex)
        else
            let lex' = skip_until_closing_quote lex '\'' in
            Error ("character literal must contain only one character", lex')
    else
        let (_, lex') = advance lex in
        Ok (c, lex')
    
let read_char lex =
    if peek lex = Some '\'' then 
        let (_, lex') = advance lex in Error ("empty character literal", lex')
    else
        let (c, lex') = advance lex in
        match resolve_char lex' c with
        | Error e -> Error e
        | Ok (c', lex'') -> 
            match close_char lex'' c' with
            | Error e -> Error e
            | Ok (final_c, lex''') -> Ok (make_token lex''' (CharLiteral final_c))

let rec read_string lex buf =
    if is_at_end lex then Error ("unterminated string literal", lex)
    else if peek lex = Some '"' then
        let (_, lex') = advance lex in
        Ok (make_token lex' (StringLiteral buf))
    else
        let (c, lex') = advance lex in
        if c = '\\' then
            if is_at_end lex' then
                Error ("unterminated string literal", lex')
            else
                let (c', lex'') = advance lex' in
                match c' with
                | 'n' -> read_string lex'' (buf ^ String.make 1 '\n')
                | 't' -> read_string lex'' (buf ^ String.make 1 '\t')
                | 'r' -> read_string lex'' (buf ^ String.make 1 '\r')
                | '\\' -> read_string lex'' (buf ^ String.make 1 '\\')
                | '\'' -> read_string lex'' (buf ^ String.make 1 '\'')
                | '"' -> read_string lex'' (buf ^ String.make 1 '"')
                | _ -> let lex''' = skip_until_closing_quote lex'' '"' in
                        Error ("invalid escape sequence", lex''')
        else
            read_string lex' (buf ^ String.make 1 c)

let rec read_hexadecimal_number_helper lex =
    match peek lex with
    | Some '.' -> Error ("hexadecimal numbers cannot have decimal points", lex)
    | Some c when is_hex_digit c ->
        let (_, lex') = advance lex in
        read_hexadecimal_number_helper lex'
    | _ -> Ok lex

let read_hexadecimal_number lex = 
    let (_, lex') = advance lex in
    match peek lex' with
    | None -> Error ("hexadecimal number must have at least one digit after 0x", lex')
    | Some c ->
        if not (is_hex_digit c) then
            Error ("hexadecimal number must have at least one digit after 0x", lex')
        else
            match read_hexadecimal_number_helper lex' with
            | Error e -> Error e
            | Ok lex'' -> Ok (make_token lex'' (IntLiteral (int_of_string (String.sub lex''.source lex''.start (lex''.curr - lex''.start)))))

let rec read_binary_number_helper lex =
    match peek lex with
    | Some '.' -> Error ("binary numbers cannot have decimal points", lex)
    | Some c when is_binary_digit c ->
        let (_, lex') = advance lex in
        read_binary_number_helper lex'
    | _ -> Ok lex

let read_binary_number lex = 
    let (_, lex') = advance lex in
    match peek lex' with
    | None -> Error ("binary number must have at least one digit after 0b", lex')
    | Some c ->
        if not (is_binary_digit c) then
            Error ("binary number must have at least one digit after 0b", lex')
        else
            match read_binary_number_helper lex' with
            | Error e -> Error e
            | Ok lex'' -> Ok (make_token lex'' (IntLiteral (int_of_string (String.sub lex''.source lex''.start (lex''.curr - lex''.start)))))

let rec read_octal_number_helper lex =
    match peek lex with
    | Some '.' -> Error ("octal numbers cannot have decimal points", lex)
    | Some c when is_octal_digit c ->
        let (_, lex') = advance lex in
        read_octal_number_helper lex'
    | _ -> Ok lex

let read_octal_number lex = 
    let (_, lex') = advance lex in
    match peek lex' with
    | None -> Error ("octal number must have at least one digit after 0o", lex')
    | Some c ->
        if not (is_octal_digit c) then
            Error ("octal number must have at least one digit after 0o", lex')
        else
            match read_octal_number_helper lex' with
            | Error e -> Error e
            | Ok lex'' -> Ok (make_token lex'' (IntLiteral (int_of_string (String.sub lex''.source lex''.start (lex''.curr - lex''.start)))))
   
let is_valid_fractional_part lex =
    if peek lex = Some '.' then
        if peek_next lex = Some '.' then
            let (_, lex') = advance lex in
            let (_, lex'') = advance lex' in
            Error ("range operator cannot follow a float literal: ", lex'')
        else
            let (_, lex') = advance lex in
            Error ("malformed float literal: ", lex')
    else
        Ok lex
        
let rec read_decimal_number_helper lex is_float =
    match peek lex with 
    | Some c when is_digit c -> 
        let (_, lex') = advance lex in 
        read_decimal_number_helper lex' is_float
    | Some '.' -> if peek_next lex = Some '.' then
            Ok (lex, is_float) (* main loop handles range operator .. *)
        else let (_, lex') = advance lex in (* fractional part*)
            read_decimal_number_helper lex' true
    | Some 'e' | Some 'E' ->  (* scientific part *)
        let (_, lex') = advance lex in 
        let lex'' = if peek lex' = Some '+' || peek lex' = Some '-' then
            let (_, lex'') = advance lex' in lex'' else lex' in
        (match peek lex'' with
        | Some c when not (is_digit c) -> Error ("malformed scientific notation", lex'')
        | _ -> read_decimal_number_helper lex'' true)
    | _ -> Ok (lex, is_float)
    
let read_decimal_number lex =
    match read_decimal_number_helper lex false with
    | Error e -> Error e
    | Ok (lex', is_float) -> if peek lex' = Some '.' && peek_next lex' = Some '.' then
        Ok (make_token lex' (IntLiteral (int_of_string (String.sub lex'.source lex'.start (lex'.curr - lex'.start))))) (* return early, 
        main loop consumes the range operator ..*)
    else
        (match is_valid_fractional_part lex' with
        | Error e -> Error e
        | Ok lex'' -> if is_float then 
            Ok (make_token lex'' (FloatLiteral (float_of_string (String.sub lex''.source lex''.start (lex''.curr - lex''.start)))))
        else
            Ok (make_token lex'' (IntLiteral (int_of_string (String.sub lex''.source lex''.start (lex''.curr - lex''.start))))))

let rec read_identifier_helper lex =
     match peek lex with
    | Some c when is_alnum c ->
        let (_, lex') = advance lex in
        read_identifier_helper lex'
    | _ -> Ok lex

let read_identifier lex =
    match read_identifier_helper lex with
    | Error e -> Error e
    | Ok lex' -> let lexeme =
        String.sub lex'.source lex'.start (lex'.curr - lex'.start) in
        match String.lowercase_ascii lexeme with
        | "true" -> Ok (make_token lex' True)
        | "false" -> Ok (make_token lex' False)
        | "fn" -> Ok (make_token lex' Fn)
        | "let" -> Ok (make_token lex' Let)
        | "mut" -> Ok (make_token lex' Mut)
        | "const" -> Ok (make_token lex' Const)
        | "if" -> Ok (make_token lex' If)
        | "else" -> Ok (make_token lex' Else)
        | "for" -> Ok (make_token lex' For)
        | "while" -> Ok (make_token lex' While)
        | "loop" -> Ok (make_token lex' Loop)
        | "match" -> Ok (make_token lex' Match)
        | "break" -> Ok (make_token lex' Break)
        | "continue" -> Ok (make_token lex' Continue)
        | "return" -> Ok (make_token lex' Return)
        | "in" -> Ok (make_token lex' In)
        | "as" -> Ok (make_token lex' As)
        | _ -> Ok (make_token lex' (Identifier (String.sub lex'.source lex'.start (lex'.curr - lex'.start))))
    
let read_number lex c = 
    if c = '0' then
        if peek lex = Some 'x' || peek lex = Some 'X' then
            read_hexadecimal_number lex
        else if peek lex = Some 'b' || peek lex = Some 'B' then
            read_binary_number lex
        else if peek lex = Some 'o' || peek lex == Some 'O' then
            read_octal_number lex
        else
            read_decimal_number lex
    else
          read_decimal_number lex

