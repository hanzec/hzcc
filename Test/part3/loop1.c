
int mything(int a)
{
  int sum;
  sum = 0;
  for (
    a=0; 
    a<10; 
    a++
  ) 
  {
    sum += a;
  }
  return a;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.