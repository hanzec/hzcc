
int exprs(int a)
{
  int b;

  3;
  a;
  a+5;
  a*b;

  exprs(3);

  "Legal!";
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.