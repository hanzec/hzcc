
int bad(int A[])
{
  int b;
  b = A[ 
    0 
    + 
    A [
        1 
        + 
        A
          [
          2
          ]
      ]

    /* ]  oops */
  ; 
  return b;
}

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 18 near text ;
//CHECK-NEXT:           Expected ']'