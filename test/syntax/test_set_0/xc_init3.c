
int foo(int a)
{
  int b=a;  

  int c, d=4, e, f=d+2, g=a*a+f;

}

int bar(int b)
{
  int c = foo(b);

}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.