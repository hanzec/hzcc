!
(
 +
 .

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token  33 Text !
//CHECK-NEXT: File __FILE__ Line     2 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     3 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line     4 Token  46 Text .