
struct thing 
{
  int random;
  float stuff[10];
  char just;
  int to_make_a_struct;
}
;

/*
  missing "struct"
*/
int 
foo
  (
  thing 
  T
  )
;

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 17 near text thing
//CHECK-NEXT:         Expected type name