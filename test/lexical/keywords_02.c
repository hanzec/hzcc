struct
truct
ruct
uct
ct
t

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 402 Text struct
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text truct
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text ruct
//CHECK-NEXT: File __FILE__ Line     4 Token 306 Text uct
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text ct
//CHECK-NEXT: File __FILE__ Line     6 Token 306 Text t