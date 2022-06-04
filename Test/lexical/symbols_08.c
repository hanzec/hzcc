!=
++
&&
*=

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 352 Text !=
//CHECK-NEXT: File __FILE__ Line     2 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     3 Token 358 Text &&
//CHECK-NEXT: File __FILE__ Line     4 Token 363 Text *=