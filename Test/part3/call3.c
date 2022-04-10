
int zero();

int one(int a);

int two(int a, int b);

int three(int a, int b, int c);

int bar(int a)
{
  int b;

  b =
    zero()
    +
    one(a)
    +
    two(
      zero(),
      one(
        zero()
        +
        a
      )
    )
    +
    three(
      two(
        one(
          zero()
          +a
        )
        ,
        zero()
      ),
      two(
        1
        ,
        4
      ),
      one(
        two(
          one(
            zero() + a
          )
          ,
          three(1,2,3)
        )
      )
    )
  ;
}


int one(int a)
{
    two(3,
        4
    ;
}

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 60 near text ;
//CHECK-NEXT:         Expected ','
