0x2
0xf
0x03bident

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: Invalid suffix 'ident' on integer constant