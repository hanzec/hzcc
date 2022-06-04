1.02
1.02e6
1.52e12

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 304 Text 1.02
//CHECK-NEXT: File __FILE__ Line     2 Token 304 Text 1.02
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text e6
//CHECK-NEXT: File __FILE__ Line     3 Token 304 Text 1.52
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text e12