
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


//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 10 near text ,
//CHECK-NEXT:         term expected