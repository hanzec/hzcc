
// basic if no braces

int main()
{
  int y;
  y = 5;
  if (y > 3) putc(10);
  y = 7;
}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.