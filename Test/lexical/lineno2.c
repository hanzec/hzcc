1








10




        15

//
//
//
//

        22

/*






*/

33

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File lineno2.c Line     1 Token 303 Text 1
//CHECK-NEXT: File lineno2.c Line    10 Token 303 Text 10
//CHECK-NEXT: File lineno2.c Line    15 Token 303 Text 15
//CHECK-NEXT: File lineno2.c Line    22 Token 303 Text 22
//CHECK-NEXT: File lineno2.c Line    33 Token 303 Text 33