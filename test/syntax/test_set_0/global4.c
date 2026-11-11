
/* Syntax error with trailing commas */

int 
  a
  , 
  b
  , 
  c
  , 
  ;


//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:     Parser error in file __FILE__ line 11 near text ;
//CHECK-NEXT:          Expected identifier
