
// unary -, !, ~

void bar(int a)
{
  int b;

  b = -a;

  a = !b;

  b = ~a;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.