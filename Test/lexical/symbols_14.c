((((((((++--==))))))));
;;;;;;;;;;;
,,,,,,,,
[[[[[[[]]]]]]]

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement --fsave_gen_test_rules

//CHECK: File __FILE__ Line     1 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     1 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     1 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     1 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     1 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     1 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     1 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     1 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     1 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     1 Token 356 Text --
//CHECK-NEXT: File __FILE__ Line     1 Token 351 Text ==
//CHECK-NEXT: File __FILE__ Line     1 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     1 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     1 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     1 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     1 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     1 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     1 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     1 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     1 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     3 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     3 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     3 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     3 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     3 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     3 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     3 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     3 Token  44 Text ,
//CHECK-NEXT: File __FILE__ Line     4 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line     4 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line     4 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line     4 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line     4 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line     4 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line     4 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line     4 Token  93 Text ]
//CHECK-NEXT: File __FILE__ Line     4 Token  93 Text ]
//CHECK-NEXT: File __FILE__ Line     4 Token  93 Text ]
//CHECK-NEXT: File __FILE__ Line     4 Token  93 Text ]
//CHECK-NEXT: File __FILE__ Line     4 Token  93 Text ]
//CHECK-NEXT: File __FILE__ Line     4 Token  93 Text ]
//CHECK-NEXT: File __FILE__ Line     4 Token  93 Text ]