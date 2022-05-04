
int foo(int a)
{
  struct pair {
    int x;
    int y;
  }
  /* Missing ; */
  int
  z;
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 9 near text int
//CHECK-NEXT:         Expected ';'