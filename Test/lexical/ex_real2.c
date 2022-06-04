56.78e-15
6.02e+23
4.1e0

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:  __FILE__ Line     1 Token 304 Text 56.78
//CHECK-NEXT: File __FILE__ Line     1 Token 306 Text e
//CHECK-NEXT: File __FILE__ Line     1 Token  45 Text -
//CHECK-NEXT: File __FILE__ Line     1 Token 303 Text 15
//CHECK-NEXT: File __FILE__ Line     2 Token 304 Text 6.02
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text e
//CHECK-NEXT: File __FILE__ Line     2 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line     2 Token 303 Text 23
//CHECK-NEXT: File __FILE__ Line     3 Token 304 Text 4.1
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text e0