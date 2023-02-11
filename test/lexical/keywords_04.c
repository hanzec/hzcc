while
while(
while (

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ Line     1 Token 517 Text while
//CHECK-NEXT: File __FILE__ Line     2 Token 517 Text while
//CHECK-NEXT: File __FILE__ Line     2 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     3 Token 517 Text while
//CHECK-NEXT: File __FILE__ Line     3 Token  40 Text (