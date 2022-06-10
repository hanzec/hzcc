1 2 3 4
56789
123
123456
1234567890
123456789012345

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement
//CHECK: File __FILE__ Line     1 Token 303 Text 1
//CHECK-NEXT: File __FILE__ Line     1 Token 303 Text 2
//CHECK-NEXT: File __FILE__ Line     1 Token 303 Text 3
//CHECK-NEXT: File __FILE__ Line     1 Token 303 Text 4
//CHECK-NEXT: File __FILE__ Line     2 Token 303 Text 56789
//CHECK-NEXT: File __FILE__ Line     3 Token 303 Text 123
//CHECK-NEXT: File __FILE__ Line     4 Token 303 Text 123456
//CHECK-NEXT: File __FILE__ Line     5 Token 303 Text 1234567890
//CHECK-NEXT: File __FILE__ Line     6 Token 303 Text 123456789012345