
// bad syntax for a complicated expression

int error(int a)
{
  int b;

  b 
  = 
  1 
  + 
    a
    -
    1 
  + 
    a
    -
    2 
  + 
    a
    -
    3 
  + 
    a
    -
    4 
  + 
    a
    *
    a 
  + 
    a
    /
    a 
  + 
    a
    %
    5 
  + 
    a
  +
  + 
    7
  ;

  return b;
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 42 near text +
//CHECK-NEXT:         term expected