
int global = 12;

int error(int a)
{
  int
  b
  =
  /* missing value */
  ,
  c
  ;
}


//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 10 near text ,
//CHECK-NEXT:         term expected