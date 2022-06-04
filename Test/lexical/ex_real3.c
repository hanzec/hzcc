1.04
1.05e6
13.45e7e8

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:  File __FILE__ Line     1 Token 304 Text 1.04
//CHECK-NEXT: File __FILE__ Line     2 Token 304 Text 1.05
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text e6
//CHECK-NEXT: File __FILE__ Line     3 Token 304 Text 13.45
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text e7e8