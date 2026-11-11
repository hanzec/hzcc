
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

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.