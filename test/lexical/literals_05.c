123.456.78


//RUN: %mycc --flexical_only --fno_color "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: __FILE__:1:0: error: Invalid string '123.456.78' for real constant