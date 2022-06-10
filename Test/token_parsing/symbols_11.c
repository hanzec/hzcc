+-*!(++)[?:];*.;
><!+!

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line     1 Token  45 Text -
//CHECK-NEXT: File __FILE__ Line     1 Token  42 Text *
//CHECK-NEXT: File __FILE__ Line     1 Token  33 Text !
//CHECK-NEXT: File __FILE__ Line     1 Token  40 Text (
//CHECK-NEXT: File __FILE__ Line     1 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     1 Token  41 Text )
//CHECK-NEXT: File __FILE__ Line     1 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line     1 Token  63 Text ?
//CHECK-NEXT: File __FILE__ Line     1 Token  58 Text :
//CHECK-NEXT: File __FILE__ Line     1 Token  93 Text ]
//CHECK-NEXT: File __FILE__ Line     1 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     1 Token  42 Text *
//CHECK-NEXT: File __FILE__ Line     1 Token  46 Text .
//CHECK-NEXT: File __FILE__ Line     1 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token  62 Text >
//CHECK-NEXT: File __FILE__ Line     2 Token  60 Text <
//CHECK-NEXT: File __FILE__ Line     2 Token  33 Text !
//CHECK-NEXT: File __FILE__ Line     2 Token  43 Text +
//CHECK-NEXT: File __FILE__ Line     2 Token  33 Text !