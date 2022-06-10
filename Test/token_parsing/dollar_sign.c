
_
ok
__

$

buh

//RUN: %mycc --flexical_only --fno_color "%s" | %FileCheck --test_rules="%s" --fsymbol_replacement
//CHECK: File __FILE__ Line     2 Token 306 Text _
//CHECK-NEXT: File __FILE__ Line     3 Token 306 Text ok
//CHECK-NEXT: File __FILE__ Line     4 Token 306 Text __
//CHECK-NEXT: File __FILE__ Line     6 Token 306 Text $
//CHECK-NEXT: File __FILE__ Line     8 Token 306 Text buh