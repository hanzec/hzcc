a//bc
de/*fghijklm*/nop
/*//*/;
/**/;
/***/;

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token 306 Text a
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text de
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text nop
//CHECK-NEXT: File __FILE__ Line     3 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     4 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     5 Token  59 Text ;