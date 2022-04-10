
int three(int a, int b, int c);

int bar(int a)
{
  int b;

  b = three(a, 2, 3, );

  return b;
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 8 near text )
//CHECK-NEXT:         term expected