
float pi = 3.14159;

int error(int a=3)
{
  /* C++ allows that, but not C. */
  int b;
}

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 4 near text =
//CHECK-NEXT:         Expected ','
