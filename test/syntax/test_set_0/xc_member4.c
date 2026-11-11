
int foo(int a)
{
  int x;

  x 
    = 
      a
        .
          ++    /* NOPE */
          member
          .
          y
          ;   
}

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 10 near text ++
//CHECK-NEXT:         Expect identifier
