
/*
  Since there's no typechecking,
  we don't need to define a struct
  to test for .
*/

int foo(int a)
{
  return a.member;
}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.