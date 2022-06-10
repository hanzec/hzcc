1.02
1.02e6
1.52e12

//RUN: %mycc --flexical_only --fno_color "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 304 Text 1.02
//CHECK-NEXT: File __FILE__ Line     2 Token 304 Text 1.02e6
//CHECK-NEXT: File __FILE__ Line     3 Token 304 Text 1.52e12