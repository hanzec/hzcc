

// nested and other tricky if-then-elses

int putc(int c);

int foo(char c)
{
  int x;
  int y;
  if (c) {
    x = 5;
    putc(65);
    if (x)
      putc(66);
    else
      putc(67);
  } else {
    x = 7;
    putc(66);
    if (x)
      if (y)
      {
        if (x < y)
          if (y < 10)
            if (x < 10)
              putc(67);
        putc(68);
      }
      else
        putc(69);
    else
      putc(70);
  }
  y = x;

  if (y)
  {
    /* Empty stmt blocks are legal */
  }
  else
  {
    putc(71);
  }
}

char evil(char x)
{
    if (x)
        putc(68);
        putc(69);   /* NOT part of the if */
    else
        putc(70);
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 52 near text else
//CHECK-NEXT:         Else has to be followed by if