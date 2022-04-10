
/*
  Since there's no typechecking,
  we don't need to define a struct
  to test for .
*/

int foo(int a)
{
  return a.member;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.