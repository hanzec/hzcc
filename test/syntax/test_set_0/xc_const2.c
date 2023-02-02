
const int sure(int a);

int also_good(int const b);

float log(const float base, float x);


//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.