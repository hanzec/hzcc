
int empty(int a)
{
    ;
}

int strange(int x)
{
  int y;

  int bad(int y);

  int z;

  /* Can't nest functions */
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 11 near text (
//CHECK-NEXT:         Function definition not allowed here