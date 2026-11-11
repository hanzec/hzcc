
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

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 9 near text int
//CHECK-NEXT:         Expected ';'