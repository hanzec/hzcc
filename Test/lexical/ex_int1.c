00
00644
009

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 303 Text 00
//CHECK-NEXT: File __FILE__ Line     2 Token 303 Text 00644
//CHECK-NEXT: File __FILE__ Line     3 Token 303 Text 009