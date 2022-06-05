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

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File keywords_15.c Line     1 Token 306 Text tricky_idents
//CHECK-NEXT: File keywords_15.c Line     2 Token 306 Text _
//CHECK-NEXT: File keywords_15.c Line     3 Token 306 Text __
//CHECK-NEXT: File keywords_15.c Line     4 Token 306 Text ___
//CHECK-NEXT: File keywords_15.c Line     5 Token 306 Text _3
//CHECK-NEXT: File keywords_15.c Line     6 Token 306 Text i
//CHECK-NEXT: File keywords_15.c Line     7 Token 306 Text i2
//CHECK-NEXT: File keywords_15.c Line     8 Token 306 Text i3
//CHECK-NEXT: File keywords_15.c Line     9 Token 306 Text i12312
//CHECK-NEXT: File keywords_15.c Line    10 Token 306 Text TrickyIdents
//CHECK-NEXT: File keywords_15.c Line    11 Token 306 Text TRICKY
//CHECK-NEXT: File keywords_15.c Line    11 Token  45 Text -
//CHECK-NEXT: File keywords_15.c Line    11 Token 306 Text IDENTS