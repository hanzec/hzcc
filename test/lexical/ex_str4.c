"howedy"
"escape \\ yup"
"unclosed, should be an error or something

"ho"

//RUN: %mycc --flexical_only --fno_color "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: __FILE__:3:41: error: Missing terminating '"' character
//CHECK-NEXT: 3 | "unclosed, should be an error or something