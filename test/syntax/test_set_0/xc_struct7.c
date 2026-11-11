
struct pair {
  int x, y;
};

/* NOT ALLOWED in C: */

pair center;


//RUN: %mycc --fsyntax_only "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      Parser error in file __FILE__ line 8 near text pair
//CHECK-NEXT:         Expected type name