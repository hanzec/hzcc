
void proto3(
  int a,
  int b
)
{
  printf ( "Hello, world!\n" ) ;
}

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
)
{
  int
  a
  ;
  float
  b
  ;
  char
  c
  ;

  printf(a, b);
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.

