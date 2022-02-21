/*
 *	The usual hello world program, but without
 *	#include <stdio.h> because the preprocessor
 *	is optional (see part 2).
 *
 */

int main()
{
  return printf("Hello, world!\n");
}

//RUN: %mycc -1 "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules
//RUN: %mycc -1 "%s" -o "%t" && cat "%t" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules
//RUN: test $(%mycc -1 "%s" -o "%t" 2&>1 | wc -l) -eq 0

//CHECK:      File __FILE__ Line     8 Token 301 Text int
//CHECK-NEXT: File __FILE__ Line     8 Token 306 Text main
//CHECK-NEXT: File __FILE__ Line     8 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     8 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     9 Token 123 Text {
//CHECK-NEXT: File __FILE__ Line    10 Token 410 Text return
//CHECK-NEXT: File __FILE__ Line    10 Token 306 Text printf
//CHECK-NEXT: File __FILE__ Line    10 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line    10 Token 305 Text "Hello, world!\n"
//CHECK-NEXT: File __FILE__ Line    10 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line    10 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line    11 Token 125 Text }

//CHECK_ERROR-EMPTY: