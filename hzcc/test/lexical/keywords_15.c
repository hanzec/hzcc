tricky_idents
_
__
___
_3
i
i2
i3
i12312
TrickyIdents
TRICKY-IDENTS

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 306 Text tricky_idents
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text _
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text __
//CHECK-NEXT: File __FILE__ Line     4 Token 306 Text ___
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text _3
//CHECK-NEXT: File __FILE__ Line     6 Token 306 Text i
//CHECK-NEXT: File __FILE__ Line     7 Token 306 Text i2
//CHECK-NEXT: File __FILE__ Line     8 Token 306 Text i3
//CHECK-NEXT: File __FILE__ Line     9 Token 306 Text i12312
//CHECK-NEXT: File __FILE__ Line    10 Token 306 Text TrickyIdents
//CHECK-NEXT: File __FILE__ Line    11 Token 306 Text TRICKY
//CHECK-NEXT: File __FILE__ Line    11 Token  45 Text -
//CHECK-NEXT: File __FILE__ Line    11 Token 306 Text IDENTS