"a"
"bcdef"
"AbCdE124"


//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 305 Text "a"
//CHECK-NEXT: File __FILE__ Line     2 Token 305 Text "bcdef"
//CHECK-NEXT: File __FILE__ Line     3 Token 305 Text "AbCdE124"