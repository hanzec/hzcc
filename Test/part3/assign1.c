
/* basic assignments with +=, -=, *=, /= */

int bar(int a)
{
  int b;
  b = 3;

  a += 2;
  a *= 10;
  a -= 6;
  a /= 3;

  b += a+1;

  a += 1+a+b+2;

  return a;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.