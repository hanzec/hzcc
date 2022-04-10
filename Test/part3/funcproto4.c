
int  func1 ( int x ) ;
int  func2 ( float y ) ;
int  func3 ( char z ) ;

void error(
  int a
  ,
  int b
  ,
);

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:   Parser error in file __FILE__ line 11 near text )
//CHECK-NEXT:         Expected type name
