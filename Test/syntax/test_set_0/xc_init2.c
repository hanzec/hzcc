
void bigfunc(int a)
{
  int b=2;
  int c, d;

  float e = 2.7182818;

  int f=6, g, h, i=9, j;

  char k='k';

}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.