'\a'
'a'
'aa

//RUN: %mycc --flexical_only --fno_color "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: __FILE__:3:0: error: unclosed char literal
//CHECK-NEXT: 3 | 'aa