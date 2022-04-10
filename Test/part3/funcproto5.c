
float /* yes */     func1               (    int      x ) ;
float /* because */ func2 /* we  */     (    float    y ) ;
float               func3 /* can */     (    char     z ) ;

void error(
  int
  a
  ,
  b
  ,
  c
);

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 10 near text b
//CHECK-NEXT:         Expected type name