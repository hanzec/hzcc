
int foo(int a)
{
  int x;

  x 
    = 
      a
      .
      w
      .
      x
      .
      .   /* oops */
      y
      .
      z
      ; 
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 14 near text .
//CHECK-NEXT:         Expect identifier
