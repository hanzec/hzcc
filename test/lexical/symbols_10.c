x++;
--y;
a[++i]--;

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token 306 Text x
//CHECK-NEXT: File __FILE__ Line     1 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     1 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     2 Token 356 Text --
//CHECK-NEXT: File __FILE__ Line     2 Token 306 Text y
//CHECK-NEXT: File __FILE__ Line     2 Token  59 Text ;
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text a
//CHECK-NEXT: File __FILE__ Line     3 Token  91 Text [
//CHECK-NEXT: File __FILE__ Line     3 Token 355 Text ++
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text i
//CHECK-NEXT: File __FILE__ Line     3 Token  93 Text ]
//CHECK-NEXT: File __FILE__ Line     3 Token 356 Text --
//CHECK-NEXT: File __FILE__ Line     3 Token  59 Text ;