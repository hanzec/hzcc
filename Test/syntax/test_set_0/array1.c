
int A[50];

int foo(int i)
{
  A[3] = i;

  A[i]++;

  A[4] += 5;

  return i;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.
