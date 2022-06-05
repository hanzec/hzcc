1.2
3.45678
910.1112


//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 304 Text 1.2
//CHECK-NEXT: File __FILE__ Line     2 Token 304 Text 3.45678
//CHECK-NEXT: File __FILE__ Line     3 Token 304 Text 910.1112