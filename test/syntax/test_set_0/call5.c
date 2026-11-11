
int three(int a, int b, int c);

int bar(int a)
{
  int b;

  b = three( , a, 2, 3);

  return b;
}

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 8 near text ,
//CHECK-NEXT:         term expected
