
// binary ==, !=, >, >=, <, <=

void bar (int a)
{
  int b;
  char c;

  c = 1==b;
  c = a!=2;
  c = 3 > b;
  c = a >= 4;
  c = 5 < b;
  c = a <= 6;
  c = 7==b;
  c = a!=8;
  c = 9 > b;
  c = a >= 10;
  c = 11 < b;
  c = a <= 12;

  c = a==b;
  c = a!=b;
  c = a > b;
  c = a >= b;
  c = a < b;
  c = a <= b;

}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.
