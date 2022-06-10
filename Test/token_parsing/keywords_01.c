c
co
con
cons
const

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 306 Text c
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text co
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text con
//CHECK-NEXT: File __FILE__ Line     4 Token 306 Text cons
//CHECK-NEXT: File __FILE__ Line     5 Token 401 Text const