
const float pi;

int nope(int z)
{
  int a,
      b,
      const c,  /* not allowed */
      d;
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:     Parser error in file __FILE__ line 8 near text const
//CHECK-NEXT:          const is not allowed here
