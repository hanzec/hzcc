"\\"
"what"
"\\\""
"the"
"\\\\\"\"\\\\"
"heck"

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 305 Text "\\"
//CHECK-NEXT: File __FILE__ Line     2 Token 305 Text "what"
//CHECK-NEXT: File __FILE__ Line     3 Token 305 Text "\\\""
//CHECK-NEXT: File __FILE__ Line     4 Token 305 Text "the"
//CHECK-NEXT: File __FILE__ Line     5 Token 305 Text "\\\\\"\"\\\\"
//CHECK-NEXT: File __FILE__ Line     6 Token 305 Text "heck"