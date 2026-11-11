
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

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.