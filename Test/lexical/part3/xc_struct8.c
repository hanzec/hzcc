
struct 
pair 
{
  x
  ;  /* Missing type names; error */
  y
  ;
}
;

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 5 near text x
//CHECK-NEXT:         Expected type name