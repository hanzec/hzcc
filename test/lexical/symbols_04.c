/
<
?
{

//RUN: %mycc --flexical_only "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement

//CHECK: File __FILE__ Line     1 Token  47 Text /
//CHECK-NEXT: File __FILE__ Line     2 Token  60 Text <
//CHECK-NEXT: File __FILE__ Line     3 Token  63 Text ?
//CHECK-NEXT: File __FILE__ Line     4 Token 123 Text {