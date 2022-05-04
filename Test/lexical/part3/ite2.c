
// if-then-else with braces and multiple statements

int foo(char c)
{
  int x; 
  int y;
  if (c) {
    x = 5;
    putc(65);
  } else {
    x = 7;
    putc(66);
  }
  y = x;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.