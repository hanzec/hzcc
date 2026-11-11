
int exprs(int a)
{
  int b;

  3;
  a;
  a+5;
  a*b;

  exprs(3);

  "Legal!";
}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.