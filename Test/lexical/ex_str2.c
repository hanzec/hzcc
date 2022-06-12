" more fun strings "
"Non-tricky escapes \t here"

"an escape: \\ by itself"
""

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 305 Text " more fun strings "
//CHECK-NEXT: File __FILE__ Line     2 Token 305 Text "Non-tricky escapes \t here"
//CHECK-NEXT: File __FILE__ Line     4 Token 305 Text "an escape: \\ by itself"
//CHECK-NEXT: File __FILE__ Line     5 Token 305 Text ""