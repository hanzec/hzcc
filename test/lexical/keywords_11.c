void
_void
void_

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 301 Text void
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text _void
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text void_