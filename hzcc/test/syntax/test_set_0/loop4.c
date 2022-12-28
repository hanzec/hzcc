
float whatever(int x)
{
  int y;

  for ( ; x; x--) {
    printf(x);
  }

  for (y=x; y>5; ) y = foo(x);


  for (x=0; ; x++) {

    exit(1);

  }

  for ( ; ; ) ;   /* spin forever I guess */

}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.