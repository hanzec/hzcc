
// Some extra credit tests

'a'
'\b'
'\t'
'd'

0
1
0x01
0x0f

0.001
1.0
1.0e-5
1.0e+6
12.34e5

"string\n"
"\\string"
"a \"fun\" string"
"an evil string \\"

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules
//RUN: %mycc --flexical_only "%s" -o "%t" && cat "%t" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

/**
* Parsing output
*/
//CHECK:      File __FILE__ Line     4 Token 302 Text 'a'
//CHECK-NEXT: File __FILE__ Line     5 Token 302 Text '\b'
//CHECK-NEXT: File __FILE__ Line     6 Token 302 Text '\t'
//CHECK-NEXT: File __FILE__ Line     7 Token 302 Text 'd'
//CHECK-NEXT: File __FILE__ Line     9 Token 303 Text 0
//CHECK-NEXT: File __FILE__ Line    10 Token 303 Text 1
//CHECK-NEXT: File __FILE__ Line    11 Token 303 Text 0x01
//CHECK-NEXT: File __FILE__ Line    12 Token 303 Text 0x0f
//CHECK-NEXT: File __FILE__ Line    14 Token 304 Text 0.001
//CHECK-NEXT: File __FILE__ Line    15 Token 304 Text 1.0
//CHECK-NEXT: File __FILE__ Line    16 Token 304 Text 1.0e-5
//CHECK-NEXT: File __FILE__ Line    17 Token 304 Text 1.0e+6
//CHECK-NEXT: File __FILE__ Line    18 Token 304 Text 12.34e5
//CHECK-NEXT: File __FILE__ Line    20 Token 305 Text "string\n"
//CHECK-NEXT: File __FILE__ Line    21 Token 305 Text "\\string"
//CHECK-NEXT: File __FILE__ Line    22 Token 305 Text "a \"fun\" string"
//CHECK-NEXT: File __FILE__ Line    23 Token 305 Text "an evil string \\"