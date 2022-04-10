
struct point {
  int x, y;
};

int foo(int a)
{
  struct inner {
    char b, c, d;
  };
  char e;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.