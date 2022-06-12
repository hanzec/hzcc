
// bad syntax for a complicated expression

int error(int a)
{
  int b;

  b = 
    ( 
      (a>0) 
      && 
      (a<10) 
    ) 
      || 
    ( 
      (a>20) 
      && 
      (a<30)

    /* )  "oops" */

  ;

  return b;
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 22 near text ;
//CHECK-NEXT:         Expected ')'
