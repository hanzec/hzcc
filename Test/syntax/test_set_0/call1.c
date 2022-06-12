
int zero();

int one(int a);

int bar(int a)
{
  int b;

  b = zero();

  b = one(4);

  return b;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.
