'a'
'A'
'x'
'Y'

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 302 Text 'a'
//CHECK-NEXT: File __FILE__ Line     2 Token 302 Text 'A'
//CHECK-NEXT: File __FILE__ Line     3 Token 302 Text 'x'
//CHECK-NEXT: File __FILE__ Line     4 Token 302 Text 'Y'