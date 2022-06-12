
int foo(int a)
{
  int x, y;

  x = a[50].m1.m2.m3.m4[y.a[12].b.c[y.w].d].m6.whatever;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.