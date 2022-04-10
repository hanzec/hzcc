
// casting

int approx(float x)
{
  int nx;

  nx = (int) x;

  return (int) ( (x-nx)*(x-nx)*x );
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.