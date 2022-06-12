
/* */

/* C style comments */

; /* Multi line
     C style comment */

3/* This should split the token */4

/* * * * * * - + % , . all ignored even / that one */

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement
//CHECK:      File __FILE__ Line     6 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     9 Token 303 Text 3
//CHECK-NEXT: File __FILE__ Line     9 Token 303 Text 4