
int moreloops(int really)
{
  int x;

  x = 3;

  do 
    x++;    /* Allowed */
  while (x<5);  

  x = 4;
}

//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.