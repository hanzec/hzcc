int
int_x
intab
integer

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ Line     1 Token 310 Text int
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text int_x
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text intab
//CHECK-NEXT: File __FILE__ Line     4 Token 306 Text integer