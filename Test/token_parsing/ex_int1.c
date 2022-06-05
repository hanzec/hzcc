00
00644
009

//RUN: %mycc --flexical_only -fno_color "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: Invalid digit '9' in octal constant