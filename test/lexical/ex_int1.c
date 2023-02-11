00
00644
009

//RUN: %mycc --flexical_only --fno_color "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      __FILE__:3:2: error: Invalid digit '9' in octal constant
//CHECK-NEXT:     3 | 009