
/* Illegal ++ chaining */

int error(int a)
{
  int b;

  b = 
    (
    ++
    a
    )
    ++
  ;

  return 
    ++
    (
    b
    ++
    )
  ;
}

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 13 near text ++
//CHECK-NEXT:         Expression is not assignable
