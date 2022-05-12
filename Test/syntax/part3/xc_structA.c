
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

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 17 near text thing
//CHECK-NEXT:         Expected type name