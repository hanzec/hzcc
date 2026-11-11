
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

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.