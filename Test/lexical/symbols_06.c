=
[
|

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token  61 Text =
//CHECK-NEXT: File __FILE__ Line     2 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line     3 Token 124 Text |