
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

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 13 near text ++
//CHECK-NEXT:         Expression is not assignable
