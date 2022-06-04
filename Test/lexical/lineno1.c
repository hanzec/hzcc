1

3 three 3.0
4
5




10

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 303 Text 1
//CHECK-NEXT: File __FILE__ Line     3 Token 303 Text 3
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text three
//CHECK-NEXT: File __FILE__ Line     3 Token 304 Text 3.0
//CHECK-NEXT: File __FILE__ Line     4 Token 303 Text 4
//CHECK-NEXT: File __FILE__ Line     5 Token 303 Text 5
//CHECK-NEXT: File __FILE__ Line    10 Token 303 Text 10