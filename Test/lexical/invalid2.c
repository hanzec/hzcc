
@

//RUN: %mycc --flexical_only --fno_color "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement
//CHECK: __FILE__:2:0: warning: Unexpected symbol, ignoring.