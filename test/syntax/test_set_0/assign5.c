
/* Another illegal chain */

int error(int a)
{
  int b;

  b = 5;

  ++
  b
  += 
  a
  ;

  return b++;
}

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 12 near text +=
//CHECK-NEXT:         Left-hand side is not assignable
