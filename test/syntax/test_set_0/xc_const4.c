
const float pi;

int nope(int z)
{
  int a,
      b,
      const c,  /* not allowed */
      d;
}

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:     Parser error in file __FILE__ line 8 near text ,
//CHECK-NEXT:          Expected ';' at end of declaration
