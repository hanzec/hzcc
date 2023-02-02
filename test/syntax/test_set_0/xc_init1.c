
int a;
int b=2;
int c;

int d=4, e=5, f=6, g, h, i=9;

float pi=3.14159;

char H='H';

int main()
{
  return b;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.