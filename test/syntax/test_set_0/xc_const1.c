
const float pi;   /* Initialization would be better here */

float const e;


int thing(int x)
{
  const int a;
  int const b;
}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.