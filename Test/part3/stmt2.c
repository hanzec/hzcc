
int func(int x)
{
  for (x=0; x<10; x++) {
    x = 15;
    continue;
  }
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.