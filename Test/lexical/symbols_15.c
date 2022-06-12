_++
_+++_--
a**=++b

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 306 Text _
//CHECK-NEXT: File __FILE__ Line     1 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text _
//CHECK-NEXT: File __FILE__ Line     2 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     2 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text _
//CHECK-NEXT: File __FILE__ Line     2 Token 356 Text --
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text a
//CHECK-NEXT: File __FILE__ Line     3 Token  42 Text *
//CHECK-NEXT: File __FILE__ Line     3 Token 363 Text *=
//CHECK-NEXT: File __FILE__ Line     3 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text b