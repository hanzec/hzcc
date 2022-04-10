
void func1(int x)
{
  x++;
  return;
}

int func2(int x)
{
  return x;
}

float func3(float x, float y)
{
  return x + y;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.