
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

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 9 near text [
//CHECK-NEXT:         term expected
