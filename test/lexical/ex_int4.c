0x2
0xf
0x03bident

//RUN: %mycc --flexical_only --fno_color "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      __FILE__:3:5: error: Invalid suffix 'ident' on integer constant
//CHECK-NEXT:      3 | 0x03bident