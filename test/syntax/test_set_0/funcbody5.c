
float empty()
{
    ;;;;;;;
}

int strange(int x)
{
  int y;

  int worse(int y)
  {
    /* Can't nest functions */
    int x;
  }

  int z;

}

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 11 near text (
//CHECK-NEXT:         Function definition not allowed here
