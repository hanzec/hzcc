==
<=
||
-=

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 351 Text ==
//CHECK-NEXT: File __FILE__ Line     2 Token 354 Text <=
//CHECK-NEXT: File __FILE__ Line     3 Token 357 Text ||
//CHECK-NEXT: File __FILE__ Line     4 Token 362 Text -=