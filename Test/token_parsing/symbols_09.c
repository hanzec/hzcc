>=
--
+=
/=


//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 353 Text >=
//CHECK-NEXT: File __FILE__ Line     2 Token 356 Text --
//CHECK-NEXT: File __FILE__ Line     3 Token 361 Text +=
//CHECK-NEXT: File __FILE__ Line     4 Token 364 Text /=