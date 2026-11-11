
struct 
pair 
{
  x
  ;  /* Missing type names; error */
  y
  ;
}
;

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 5 near text x
//CHECK-NEXT:         Expected type name