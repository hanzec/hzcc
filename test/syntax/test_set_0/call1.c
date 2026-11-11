
int zero();

int one(int a);

int bar(int a)
{
  int b;

  b = zero();

  b = one(4);

  return b;
}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.
