1.11

2.3e-4.5


//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:  File __FILE__ Line     1 Token 304 Text 1.11
//CHECK-NEXT: File __FILE__ Line     3 Token 304 Text 2.3
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text e
//CHECK-NEXT: File __FILE__ Line     3 Token  45 Text -
//CHECK-NEXT: File __FILE__ Line     3 Token 304 Text 4.5