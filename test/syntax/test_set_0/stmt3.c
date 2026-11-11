
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

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.