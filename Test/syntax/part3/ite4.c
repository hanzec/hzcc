
// bad if syntax

int badone(int x)
{
  int y;

  if x
  {
    badone(1);
  }
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 8 near text x
//CHECK-NEXT:         Expected '(
