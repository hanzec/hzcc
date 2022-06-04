-2.7
+3.4

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token  45 Text -
//CHECK-NEXT: File __FILE__ Line     1 Token 304 Text 2.7
//CHECK-NEXT: File __FILE__ Line     2 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line     2 Token 304 Text 3.4