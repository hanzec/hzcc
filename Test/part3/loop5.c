
int toxic_positivity()
{
  while (1) {
    putc(72);
    putc(101);
    putc(108);
    putc(108);
    putc(111);
    putc(33);
    putc(10);
  }
}


int truncated(int a)
{
    int i;
    for (i=0; i<5; i++)
    {
        printf("Iteration %d\n", i);

/* And, we chop off the end of the function. */

//RUN: %mycc -3 "%s" 2>&1 | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK:      Parser error in file __FILE__ line 20 near text {
//CHECK-NEXT:         Unmatched '{', need '}'
