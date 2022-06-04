0X30
5
0077
9
0xafb
0x8C3

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 303 Text 0
//CHECK-NEXT: File __FILE__ Line     1 Token 306 Text X30
//CHECK-NEXT: File __FILE__ Line     2 Token 303 Text 5
//CHECK-NEXT: File __FILE__ Line     3 Token 303 Text 0077
//CHECK-NEXT: File __FILE__ Line     4 Token 303 Text 9
//CHECK-NEXT: File __FILE__ Line     5 Token 303 Text 0
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text xafb
//CHECK-NEXT: File __FILE__ Line     6 Token 303 Text 0
//CHECK-NEXT: File __FILE__ Line     6 Token 306 Text x8C3