00
00644
009

//RUN: %mycc --flexical_only -fno_color "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: Invalid digit '9' in octal constant