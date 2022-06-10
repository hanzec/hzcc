+
+=
++
++=
+++
+++=
++++

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line     2 Token 361 Text +=
//CHECK-NEXT: File __FILE__ Line     3 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     4 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     4 Token  61 Text =
//CHECK-NEXT: File __FILE__ Line     5 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     5 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line     6 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     6 Token 361 Text +=
//CHECK-NEXT: File __FILE__ Line     7 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     7 Token 355 Text ++