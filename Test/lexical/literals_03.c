2
-3
+4

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File literals_03.c Line     1 Token 303 Text 2
//CHECK-NEXT: File literals_03.c Line     2 Token  45 Text -
//CHECK-NEXT: File literals_03.c Line     2 Token 303 Text 3
//CHECK-NEXT: File literals_03.c Line     3 Token  43 Text +
//CHECK-NEXT: File literals_03.c Line     3 Token 303 Text 4