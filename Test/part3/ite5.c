
// bad if syntax

void bad(int x)
{
  if (x) 
  {
    x = 1;
  } 
  else 
  {
    x = 2;
  } 
  else 
  {
    x = 3;
  }
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 14 near text else
//CHECK-NEXT:         If statement cannot have multiple else statements