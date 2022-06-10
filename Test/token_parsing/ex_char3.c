'\a'
'\ab'

//RUN: %mycc --flexical_only -fno_color "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: Multi-character character constant