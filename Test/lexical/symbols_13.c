>
>=
>==
!==
|
||
|||
&
&&
&&&

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token  62 Text >
//CHECK-NEXT: File __FILE__ Line     2 Token 353 Text >=
//CHECK-NEXT: File __FILE__ Line     3 Token 353 Text >=
//CHECK-NEXT: File __FILE__ Line     3 Token  61 Text =
//CHECK-NEXT: File __FILE__ Line     4 Token 352 Text !=
//CHECK-NEXT: File __FILE__ Line     4 Token  61 Text =
//CHECK-NEXT: File __FILE__ Line     5 Token 124 Text |
//CHECK-NEXT: File __FILE__ Line     6 Token 357 Text ||
//CHECK-NEXT: File __FILE__ Line     7 Token 357 Text ||
//CHECK-NEXT: File __FILE__ Line     7 Token 124 Text |
//CHECK-NEXT: File __FILE__ Line     8 Token  38 Text &
//CHECK-NEXT: File __FILE__ Line     9 Token 358 Text &&
//CHECK-NEXT: File __FILE__ Line    10 Token 358 Text &&
//CHECK-NEXT: File __FILE__ Line    10 Token  38 Text &