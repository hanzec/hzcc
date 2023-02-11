
`

//RUN: %mycc --flexical_only --fno_color "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement
//CHECK: __FILE__:2:0: warning: Ignoring unexpected symbol: `
//CHECK-NEXT: 2 | `