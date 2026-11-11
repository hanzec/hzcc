


void
error
(
 const
 a,
const
const   /* double secret const? */
x
)
;

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 8 near text a
//CHECK-NEXT:         Expected type name
