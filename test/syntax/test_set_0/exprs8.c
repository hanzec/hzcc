
// casting

int approx(float x)
{
  int nx;

  nx = (int) x;

  return (int) ( (x-nx)*(x-nx)*x );
}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.