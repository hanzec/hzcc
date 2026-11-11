
void proto3(
  int a,
  int b
);

int proto4(
  float u
  ,
  int v
  ,
  char w
  ,
  int x
  ,
  int y
  ,
  float z
);

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.