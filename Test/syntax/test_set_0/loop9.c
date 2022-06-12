
int moreloops(int really)
{
  int x;

  x = 3;

  do {
    x++;
    while (x%3) {
      x++;
    }
    x--;
    do 
    {
      /* This is legal */
    }
    while (x++);
  } while (x<5);  

  x = 4;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.