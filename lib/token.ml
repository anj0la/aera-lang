open Position
open Int64

type token_kind  = 
| Identifier of string
| IntLiteral of Int64.t
| FloatLiteral of float
| CharLiteral of char
| StringLiteral of string
| True
| False
| Fn
| Let
| Mut
| Const
| If
| Else
| For
| While
| Loop
| Match
| Break
| Continue
| Return
| In
| As
| LeftParen
| RightParen
| LeftBrace
| RightBrace
| LeftBracket
| RightBracket
| Comma
| Period
| Semicolon
| Colon
| Grave
| Newline
| LessLessEqual
| GreaterGreaterEqual
| PeriodPeriodEqual
| AmpAmp
| PipePipe
| EqualEqual
| ExclaimEqual
| LessEqual
| GreaterEqual
| LessLess
| GreaterGreater
| PlusEqual
| MinusEqual
| StarEqual
| SlashEqual
| PercentEqual
| AmpEqual
| PipeEqual
| CaretEqual
| MinusGreater
| PeriodPeriod
| EqualGreater
| QuestionQuestion
| Amp
| Pipe
| Caret
| Plus
| Minus
| Star
| Slash
| Percent
| Question
| At
| Exclaim
| Less
| Greater
| Equal
| LineComment  (* # *)
| BlockComment (* <# ... #>*)
| Illegal
| EOF

type token = {
    kind: token_kind;
    lexeme: string;
    pos: position;
}