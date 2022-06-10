float
floaty
float y
ifloat

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 301 Text float
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text floaty
//CHECK-NEXT: File __FILE__ Line     3 Token 301 Text float
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text y
//CHECK-NEXT: File __FILE__ Line     4 Token 306 Text ifloat