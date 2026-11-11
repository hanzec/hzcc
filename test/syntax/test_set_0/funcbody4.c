
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

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 11 near text (
//CHECK-NEXT:         Function definition not allowed here