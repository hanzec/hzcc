
int bar(int A[])
{
  return A[5];
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.