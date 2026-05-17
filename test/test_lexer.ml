open Test_lexer_helper

let test_let_binding () =
    let tokens = lex_string "let x = 5" in
    Test_lexer_helper.expect_tokens tokens [
        let_ 1 1;
        identifier "x" 1 5;
        equal 1 7;
        int_lit 5L 1 9;
        eof 1 10;
    ]

let test_if_expr () =
    let tokens = lex_string "if true { do_something() }" in
    Test_lexer_helper.expect_tokens tokens [
        if_ 1 1;
        true_lit 1 4;
        left_brace 1 9;
        identifier "do_something" 1 11;
        left_paren 1 23;
        right_paren 1 24;
        right_brace 1 26;
        eof 1 27;
    ]

let () =
  Alcotest.run "Lexer" [
    ("control", [
      Alcotest.test_case "let binding" `Quick test_let_binding;
      Alcotest.test_case "if expression" `Quick test_if_expr;
    ]);
  ]