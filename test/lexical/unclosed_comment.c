/*
*	This is an unclosed comment.
*	We should get an error message at the comment start.

//RUN: %mycc --flexical_only --fno_color "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement
//CHECK: __FILE__:1:0: error: Unclosed comment, expected to match this '/*'
