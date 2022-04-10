
/* Syntax error with leading comma */

int 
  , 
  a
  , 
  b
  , 
  c
  ;

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 5 near text ,
//CHECK-NEXT:         Expected identifier
