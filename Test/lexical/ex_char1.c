'a'
'\a'
'b'
'\b'
't'
'\t'
'n'
'\n'

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 302 Text 'a'
//CHECK-NEXT: File __FILE__ Line     2 Token 302 Text '\a'
//CHECK-NEXT: File __FILE__ Line     3 Token 302 Text 'b'
//CHECK-NEXT: File __FILE__ Line     4 Token 302 Text '\b'
//CHECK-NEXT: File __FILE__ Line     5 Token 302 Text 't'
//CHECK-NEXT: File __FILE__ Line     6 Token 302 Text '\t'
//CHECK-NEXT: File __FILE__ Line     7 Token 302 Text 'n'
//CHECK-NEXT: File __FILE__ Line     8 Token 302 Text '\n'