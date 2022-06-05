123.456.78


//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 304 Text 123.456
//CHECK-NEXT: File __FILE__ Line     1 Token  46 Text .
//CHECK-NEXT: File __FILE__ Line     1 Token 303 Text 78