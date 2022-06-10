"this is a \n string"
"this is an \"evil\" string"
"ordinary"

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:File __FILE__ Line     1 Token 305 Text "this is a \n string"
//CHECK-NEXT: File __FILE__ Line     2 Token 305 Text "this is an \"evil\" string"
//CHECK-NEXT: File __FILE__ Line     3 Token 305 Text "ordinary"