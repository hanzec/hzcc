
// ternary ? :

void bar(int a)
{
  int b;
  int c;

  b = a ? 5 : 6;

  c = (b>5) ? (b+1) : (b-1);

  a = (c>1) ?  (b ? 1 : 2) : 3;

  c = a
        ?
          b
            ?
              c
                ?
                  1
                :
                  2
            :
              !c
                ?
                  3
                :
                  4
        :
          !b
            ?
              c
                ?
                  5
                :
                  6
            :
              !c
                ?
                  7
                :
                  8
      ;
}

void tricky(int x)
{
    int a;
    a = 2 : x;
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 50 near text :
//CHECK-NEXT:         Expected ';'