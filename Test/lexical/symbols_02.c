    ;
    >
    ]
    }


//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  62 Text >
//CHECK-NEXT: File __FILE__ Line     3 Token  93 Text ]
//CHECK-NEXT: File __FILE__ Line     4 Token 125 Text }