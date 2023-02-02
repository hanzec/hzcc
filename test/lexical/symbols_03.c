%
)
,

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token  37 Text %
//CHECK-NEXT: File __FILE__ Line     2 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     3 Token  44 Text ,