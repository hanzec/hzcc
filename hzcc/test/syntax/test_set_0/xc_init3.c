
int foo(int a)
{
  int b=a;  

  int c, d=4, e, f=d+2, g=a*a+f;

}

int bar(int b)
{
  int c = foo(b);

}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.