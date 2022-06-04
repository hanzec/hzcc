' '
'a'
'3'
'r'
';'
'_'
'*'
'&'


//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 302 Text ' '
//CHECK-NEXT: File __FILE__ Line     2 Token 302 Text 'a'
//CHECK-NEXT: File __FILE__ Line     3 Token 302 Text '3'
//CHECK-NEXT: File __FILE__ Line     4 Token 302 Text 'r'
//CHECK-NEXT: File __FILE__ Line     5 Token 302 Text ';'
//CHECK-NEXT: File __FILE__ Line     6 Token 302 Text '_'
//CHECK-NEXT: File __FILE__ Line     7 Token 302 Text '*'
//CHECK-NEXT: File __FILE__ Line     8 Token 302 Text '&'