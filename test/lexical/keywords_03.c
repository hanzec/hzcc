
fort
for3
for
vfor

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     2 Token 306 Text fort
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text for3
//CHECK-NEXT: File __FILE__ Line     4 Token 508 Text for
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text vfor
