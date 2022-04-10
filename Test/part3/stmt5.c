
void empty(int a)
{
  ;
  ;
  ;
  ;
  /* Empty expressions are legal in C */
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.