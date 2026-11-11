
void empty(int a)
{
  ;
  ;
  ;
  ;
  /* Empty expressions are legal in C */
}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.