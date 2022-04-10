
int bad_do_while()
{
  int x;
  do {
    x++;
  } while 1;  /* parens required */
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 7 near text 1
//CHECK-NEXT:         Expected '('