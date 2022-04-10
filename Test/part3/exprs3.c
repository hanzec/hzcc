
// binary |, &, ||, &&

void bar(int a)
{
  int b;

  b = a | 1;
  a = b & 2;
  b = a || 3;
  a = b && 4;
  b = a | b;
  a = b & a;
  b = a || b;
  a = b && a;
  b = 5 | 6;
  a = 7 & b;
  b = 8 || a;
  a = 9 && b;

}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.