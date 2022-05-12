
/* Lexer should handle this easily */

int
  a
  ,
  b
  ,
  c
  ;

float v[100], w;
float x, y[100];

float s[100], t[100], u[100];

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.