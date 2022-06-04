0x2
0xf
0x03bident

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 303 Text 0
//CHECK-NEXT: File __FILE__ Line     1 Token 306 Text x2
//CHECK-NEXT: File __FILE__ Line     2 Token 303 Text 0
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text xf
//CHECK-NEXT: File __FILE__ Line     3 Token 303 Text 0
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text x03bident
