"empty:"
""
"Lots of strange symbols *!$&^@(#)$*%^:;>,><~`"
"What does /* this */ do?  It's still a string!"
"// this is pretty evil, right"

"Should give an error for unclosed string.

//RUN: %mycc --flexical_only --fno_color "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement
//CHECK:      __FILE__:7:41: error: Missing terminating '"' character
//CHECK-NEXT:       7 | "Should give an error for unclosed string.