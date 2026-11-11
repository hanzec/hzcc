
char func1(int x);
char func2(float y);
char func3(char z);

void error(
  ,
  int a
  ,
  int b
);

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 7 near text ,
//CHECK-NEXT:         Expected type name
