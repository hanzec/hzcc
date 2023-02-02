
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

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 10 near text ++
//CHECK-NEXT:         Expect identifier
