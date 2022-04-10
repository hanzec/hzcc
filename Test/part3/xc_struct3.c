
struct S1 {
};

struct S2 {
  struct S1 nothing;
  int something;
};

struct S3 {
  struct S1 first;
  struct S2 second;
  int third;
};

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.