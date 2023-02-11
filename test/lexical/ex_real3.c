1.04
1.05e6
13.45e7e8

//RUN: %mycc --flexical_only --fno_color "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      __FILE__:3:0: error: Invalid string '13.45e7e8' for real constant
//CHECK-NEXT:   3 | 13.45e7e8