/*
 *	The usual hello world program, but without
 *	#include <stdio.h> because the preprocessor
 *	is optional (see test_set_0 2).
 *
 */

int main()
{
  return printf("Hello, world!\n");
}

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

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