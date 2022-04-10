
const float pi;   /* Initialization would be better here */

float const e;


int thing(int x)
{
  const int a;
  int const b;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.