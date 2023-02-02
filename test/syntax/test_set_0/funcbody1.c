
void proto1()
{
  int
  a
  ;
}

void proto2(int a)
{
  int
  b
  ,
  c
  ,
  d
  ;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.