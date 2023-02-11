con
continue
constinue

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 306 Text con
//CHECK-NEXT: File __FILE__ Line     2 Token 502 Text continue
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text constinue