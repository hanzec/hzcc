
int another(int x)
{
  int y;

  while ( x > 10 ) x--;

  while ( x < y )
    while ( y > 10 )
    {
      y--;
      x++;
    }
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.