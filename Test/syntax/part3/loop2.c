
int badthing(int a)
{

  for (
    a=0; 
    a==1; 
    a=2; 
    a=3; 
    a=4 
  ) 
  {
    a=5;
  }
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 8 near text ;
//CHECK-NEXT:          Expected ')'