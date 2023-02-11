char
const char
char const

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ Line     1 Token 311 Text char
//CHECK-NEXT: File __FILE__ Line     2 Token 400 Text const
//CHECK-NEXT: File __FILE__ Line     2 Token 311 Text char
//CHECK-NEXT: File __FILE__ Line     3 Token 311 Text char
//CHECK-NEXT: File __FILE__ Line     3 Token 400 Text const