
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

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.