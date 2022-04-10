
int error(int i)
{
  int A[10];
  int b;

  b = A
    [ 
    [
    i
    ] 
    ]
  ;

  return b;
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 9 near text [
//CHECK-NEXT:         term expected
