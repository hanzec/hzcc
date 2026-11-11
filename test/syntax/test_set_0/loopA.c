
int bad_do_while()
{
  int x;
  do {
    x++;
  } while 1;  /* parens required */
}

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 7 near text 1
//CHECK-NEXT:         Expected '('