
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

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.