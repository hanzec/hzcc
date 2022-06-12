case
default
switch

casecase
defaultcase
switchdefaultcase

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 412 Text case
//CHECK-NEXT: File __FILE__ Line     2 Token 413 Text default
//CHECK-NEXT: File __FILE__ Line     3 Token 411 Text switch
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text casecase
//CHECK-NEXT: File __FILE__ Line     6 Token 306 Text defaultcase
//CHECK-NEXT: File __FILE__ Line     7 Token 306 Text switchdefaultcase