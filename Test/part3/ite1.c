
// basic if no braces

int main()
{
  int y;
  y = 5;
  if (y > 3) putc(10);
  y = 7;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.