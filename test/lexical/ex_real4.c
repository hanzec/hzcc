1.11

2.3e-4.5


//RUN: %mycc --flexical_only --fno_color "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: __FILE__:3:0: error: Invalid string '2.3e-4.5' for real constant
//CHECK-NEXT: 3 | 2.3e-4.5