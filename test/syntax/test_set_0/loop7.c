
void bad()
{
  int x;

  while 
    ( /* nope */ )
  {
    x++;
  }
}

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 7 near text )
//CHECK-NEXT:         term expected