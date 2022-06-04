"a"
"bcdef"
"AbCdE124"


//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File literals_07.c Line     1 Token 305 Text "a"
//CHECK-NEXT: File literals_07.c Line     2 Token 305 Text "bcdef"
//CHECK-NEXT: File literals_07.c Line     3 Token 305 Text "AbCdE124"