
// /* These are the trickier ones
;
/* Doesn't count: // */
still inside the comment even vi gets this wrong
*/
;
// /*
;
// */

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK:  File __FILE__ Line     3 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text still
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text inside
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text the
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text comment
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text even
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text vi
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text gets
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text this
//CHECK-NEXT: File __FILE__ Line     5 Token 306 Text wrong
//CHECK-NEXT: File __FILE__ Line     6 Token  42 Text *
//CHECK-NEXT: File __FILE__ Line     6 Token  47 Text /
//CHECK-NEXT: File __FILE__ Line     7 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     9 Token  59 Text ;