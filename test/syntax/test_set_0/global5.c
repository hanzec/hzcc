
/* Syntax error with leading comma */

int 
  , 
  a
  , 
  b
  , 
  c
  ;

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 5 near text ,
//CHECK-NEXT:         Expected identifier
