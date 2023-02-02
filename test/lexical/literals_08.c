"hello world"
"this is a string"
"          "
"3+4=7; right?"

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 305 Text "hello world"
//CHECK-NEXT: File __FILE__ Line     2 Token 305 Text "this is a string"
//CHECK-NEXT: File __FILE__ Line     3 Token 305 Text "          "
//CHECK-NEXT: File __FILE__ Line     4 Token 305 Text "3+4=7; right?"