// Single line comment
;
// Nested // single // line // comments // no // big // deal //
;

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement
//CHECK:      File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     4 Token  59 Text ;