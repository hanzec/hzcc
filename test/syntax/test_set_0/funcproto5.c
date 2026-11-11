
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

//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 10 near text b
//CHECK-NEXT:         Expected type name