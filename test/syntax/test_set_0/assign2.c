
/* Basic ++, -- */

int bar(int a)
{
  int b;

  b = a;

  a++;
  ++b;

  a--;
  --b;

  return a == b;
}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.