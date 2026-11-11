
const int sure(int a);

int also_good(int const b);

float log(const float base, float x);


//RUN: %mycc --fsyntax_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:      File __FILE__ is syntactically correct.