
int func(int x)
{
  for (x=0; x<10; x++) {
    x = 15;
    continue;
  }
}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.