
// compicated comparisons with ( )

int bar(int a)
{
  int b;
  int c;

  c = ( (a>b) && (a>c) && (a>0) )
      ||
      ( (a<b) && (a<c) && (a<0) )
      ;


  return (a>=2) && (a<=10) && (a!=b) && (a==c) && (a>b) && (b<c) ;
}

//RUN: %mycc -3 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      File __FILE__ is syntactically correct.